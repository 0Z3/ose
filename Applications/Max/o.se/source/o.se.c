/*
  Copyright (c) 2019-21 John MacCallum
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

/* NOTE: THIS CODE IS NOT THREAD-SAFE */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ext.h"
#include "ext_obex.h"
#include "ext_obex_util.h"
#include "ext_critical.h"

#include "ose.h"
#include "ose_context.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose_assert.h"
#include "ose_vm.h"
#include "ose_symtab.h"
#include "ose_match.h"
#include "ose_print.h"
#include "sys/ose_load.h"

#define OVM_INLET_INPUT 0
#define OVM_INLET_STACK 1
#define OVM_INLET_ENV 2
#define OVM_OUTLET_OUTPUT 0
#define OVM_OUTLET_STACK 1
#define OVM_OUTLET_ENV 2

typedef struct _ovm {
	t_object ob;
	t_critical lock;
	
	long total_num_inlets;
	long num_vm_inlets;
	long num_user_inlets;
	long inlet;
	void **proxy;
	t_symbol **user_inlet_addrs;
	long *user_inlet_argcs;
	t_atom **user_inlet_argvs;
	
	void *prefix_delegation_outlet;
	void *route_delegation_outlet;
	t_symbol **route_addrs;
	long num_route_outlets;
	void **route_outlets;
	long num_vm_outlets;
	void **vm_outlets;

	char *prefix;
	int32_t prefixlen;
	
	char *bytes;
	ose_bundle bundle, osevm, vm_i, vm_s, vm_e, vm_c, vm_d, vm_o, vm_g;
} ovm;

t_class *ovm_class;

t_symbol *ps_FullPacket;

static void ovm_bindUserArgs(ovm *x)
{
	for(int i = 0; i < x->num_user_inlets; i++){
		ose_pushString(x->vm_e, x->user_inlet_addrs[i]->s_name);
		while(ose_rollMatch_impl(x->vm_e)){
			ose_drop(x->vm_e);
			ose_pushString(x->vm_e, x->user_inlet_addrs[i]->s_name);
		}
		ose_drop(x->vm_e);
	}
	if(x->num_route_outlets){
		ose_pushString(x->vm_e, "/outlets");
		while(ose_rollMatch_impl(x->vm_e)){
			ose_drop(x->vm_e);
			ose_pushString(x->vm_e, "/outlets");
		}
		ose_drop(x->vm_e);
	}
	
	for(int i = 0; i < x->num_user_inlets; i++){
		ose_pushMessage(x->vm_e,
				x->user_inlet_addrs[i]->s_name,
				(int32_t)strlen(x->user_inlet_addrs[i]->s_name),
				0);
		for(int j = 0; j < x->user_inlet_argcs[i]; j++){
			t_atom *a = x->user_inlet_argvs[i] + j;
			switch(atom_gettype(a)){
			case A_LONG:
				ose_pushInt32(x->vm_e, (int32_t)atom_getlong(a));
				break;
			case A_FLOAT:
				ose_pushFloat(x->vm_e, atom_getfloat(a));
				break;
			case A_SYM:
				ose_pushString(x->vm_e, atom_getsym(a)->s_name);
				break;
			}
			ose_push(x->vm_e);
		}
	}

	if(x->num_route_outlets){
		ose_pushMessage(x->vm_e,
				"/outlets",
				strlen("/outlets"),
				0);
		for(int i = 0; i < x->num_route_outlets; i++){
			ose_pushString(x->vm_e, x->route_addrs[i]->s_name);
			ose_push(x->vm_e);
		}
	}

	if(x->prefix){
		ose_pushString(x->vm_e, "/prefix");
		while(ose_rollMatch_impl(x->vm_e)){
			ose_drop(x->vm_e);
			ose_pushString(x->vm_e, "/prefix");
		}
		ose_drop(x->vm_e);
		ose_pushMessage(x->vm_e,
				"/prefix",
				strlen("/prefix"),
				1, OSETT_STRING, x->prefix);
	}
}

static void ovm_outlet_bundle(void *outlet, t_symbol *msg, ose_bundle bundle)
{
	t_atom out[2];
	atom_setlong(out, ose_readInt32(bundle, -4));
	atom_setlong(out + 1, (long)ose_getBundlePtr(bundle));
	outlet_anything(outlet, msg, 2, out);
}

static void ovm_outlet_bundleElem(void *outlet,
				  t_symbol *msg,
				  ose_bundle bundle,
				  int32_t offset)
{
	t_atom out[2];
	atom_setlong(out, ose_readInt32(bundle, offset));
	atom_setlong(out + 1, (long)(ose_getBundlePtr(bundle) + offset + 4));
	outlet_anything(outlet, msg, 2, out);
}

static void ovm_outputBundles(ovm *x)
{
	if(x->prefix){
		ovm_outlet_bundle(x->prefix_delegation_outlet,
				  ps_FullPacket, x->vm_g);
		ose_clear(x->vm_g);
	}
	for(int i = 0; i < x->num_route_outlets; i++){
		ose_pushBundle(x->vm_d);
		int32_t dbo = ose_getLastBundleElemOffset(x->vm_d);		
		const char * const op = ose_getBundlePtr(x->vm_o);
		int32_t os = ose_readInt32(x->vm_o, -4);
		const char * const addr = x->route_addrs[i]->s_name;
		int32_t o = OSE_BUNDLE_HEADER_LEN;
		int32_t s;
		int32_t route_bundle_size = 0;
		while(o < os){
			s = ose_readInt32(op, o);
			int32_t matched = s & 0x80000000;
			s &= 0x7FFFFFFF;
			ose_writeInt32(op, o, s);
			int32_t po = 0, ao = 0;
		        int32_t r = ose_match_pattern(op + o + 4, addr,
						      &po, &ao);
			if(r & OSE_MATCH_ADDRESS_COMPLETE){
				int32_t ns = ose_routeElemAtOffset(o,
								   x->vm_o,
								   po,
								   x->vm_d);
				route_bundle_size += ns + 4;
				matched = 0x80000000;
			}
			ose_writeInt32(op, o, s | matched);
			o += s + 4;
		}
		ose_writeInt32(x->vm_d, dbo,
			       route_bundle_size + OSE_BUNDLE_HEADER_LEN);
	}
	// delegation
	{
		ose_pushBundle(x->vm_d);
		int32_t dbo = ose_getLastBundleElemOffset(x->vm_d);
		const char * const op = ose_getBundlePtr(x->vm_o);
		int32_t os = ose_readInt32(x->vm_o, -4);
		int32_t o = OSE_BUNDLE_HEADER_LEN;
		int32_t s;
		int32_t route_bundle_size = 0;
		while(o < os){
			s = ose_readInt32(op, o);
			int32_t matched = s & 0x80000000;
			s &= 0x7FFFFFFF;
			ose_writeInt32(op, o, s);
			if(!matched){
				ose_copyElemAtOffset(o, x->vm_o, x->vm_d);
				route_bundle_size += s + 4;
			}
			o += s + 4;
		}
		ose_writeInt32(x->vm_d, dbo,
			       route_bundle_size + OSE_BUNDLE_HEADER_LEN);
		ovm_outlet_bundleElem(x->route_delegation_outlet,
				      ps_FullPacket, x->vm_d, dbo);
		ose_drop(x->vm_d);
	}
	for(int i = x->num_route_outlets - 1; i >= 0; i--){
		ovm_outlet_bundleElem(x->route_outlets[i],
				      ps_FullPacket,
				      x->vm_d,
				      ose_getLastBundleElemOffset(x->vm_d));
		ose_drop(x->vm_d);
	}
	ovm_outlet_bundle(x->vm_outlets[OVM_OUTLET_ENV], ps_FullPacket, x->vm_e);
	ovm_outlet_bundle(x->vm_outlets[OVM_OUTLET_STACK], ps_FullPacket, x->vm_s);
	ovm_outlet_bundle(x->vm_outlets[OVM_OUTLET_OUTPUT], ps_FullPacket, x->vm_o);
}

void ovm_popInputToControl(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_g = ose_enter(osevm, "/_g");
        ose_pushString(vm_e, "/prefix");
	const int32_t r = ose_pickMatch_impl(vm_e);
	if(r){
		const char * const pfx = ose_peekString(vm_e);
		const char * const addr = ose_peekAddress(vm_i);
		int32_t po = 0, ao = 0;
		const int32_t r = ose_match_pattern(addr, pfx, &po, &ao);
		if(r & OSE_MATCH_ADDRESS_COMPLETE){
			const int32_t io = ose_getLastBundleElemOffset(vm_i);
			ose_routeElemAtOffset(io, vm_i, po, vm_c);
			ose_drop(vm_i);
		}else{
			ose_moveElem(vm_i, vm_g);
		}
		ose_drop(vm_e);
	}else{
		ose_drop(vm_e);
		osevm_popInputToControl(osevm);
	}
}

void ovm_FullPacket(ovm *x, long _len, long _ptr)
{
    int32_t len = (int32_t)_len;
	char *ptr = (char *)_ptr;
	const long inlet = proxy_getinlet((t_object *)x);
	if(inlet == OVM_INLET_INPUT){
		osevm_inputTopLevel(x->osevm, len, ptr);
		ovm_bindUserArgs(x);
		osevm_run(x->osevm);
		ovm_outputBundles(x);
	}else if(inlet == OVM_INLET_STACK){
		int32_t s = ose_readInt32(x->vm_s, -4);
		ose_addToSize(x->vm_s, len - OSE_BUNDLE_HEADER_LEN);
		memcpy(ose_getBundlePtr(x->vm_s) + s,
		       ptr + OSE_BUNDLE_HEADER_LEN,
		       len - OSE_BUNDLE_HEADER_LEN);
	}else if(inlet == OVM_INLET_ENV){
		int32_t o = OSE_BUNDLE_HEADER_LEN;
		int32_t s;
		while(o < len){
			s = ose_ntohl(*((int32_t *)(ptr + o)));
			ose_pushString(x->vm_e, ptr + o + 4);
			while(ose_rollMatch_impl(x->vm_e)){
				ose_drop(x->vm_e);
				ose_pushString(x->vm_e, ptr + o + 4);
			}
			ose_drop(x->vm_e);
			o += s + 4;
		}
		s = ose_readInt32(x->vm_e, -4);
		ose_addToSize(x->vm_e, len - OSE_BUNDLE_HEADER_LEN);
		memcpy(ose_getBundlePtr(x->vm_e) + s,
		       ptr + OSE_BUNDLE_HEADER_LEN,
		       len - OSE_BUNDLE_HEADER_LEN);
	}else{

	}
}

void ovm_bang(ovm *x)
{
	ose_pushInt32(x->vm_i, 666);
}

void ovm_anything(ovm *x, t_symbol *msg, long argc, t_atom *argv)
{
	const long inlet = proxy_getinlet((t_object *)x);
	if(inlet < x->num_vm_inlets){
		object_error((t_object *)x, "unknown message %s\n", msg);
		return;
	}
	long idx = inlet - x->num_vm_inlets;
	long oldargc = x->user_inlet_argcs[idx];
	if(argc + 1 <= oldargc){
		if(x->user_inlet_argvs[idx]){
			x->user_inlet_argvs[idx] = realloc(x->user_inlet_argvs[idx],
							   (argc + 1) * sizeof(t_atom));
		}else{
			x->user_inlet_argvs[idx] = malloc((argc + 1) * sizeof(t_atom));
		}
	}
	x->user_inlet_argcs[idx] = argc + 1;
	atom_setsym(x->user_inlet_argvs[idx], msg);
	memcpy(x->user_inlet_argvs[idx] + 1, argv, argc * sizeof(t_atom));
}

void ovm_list(ovm *x, t_symbol *msg, long argc, t_atom *argv)
{
	const long inlet = proxy_getinlet((t_object *)x);
	if(inlet < x->num_vm_inlets){
		object_error((t_object *)x, "unknown message (list)\n");
		return;
	}
	long idx = inlet - x->num_vm_inlets;
	long oldargc = x->user_inlet_argcs[idx];
	if(argc <= oldargc){
		if(x->user_inlet_argvs[idx]){
			x->user_inlet_argvs[idx] = realloc(x->user_inlet_argvs[idx],
							   argc * sizeof(t_atom));
		}else{
			x->user_inlet_argvs[idx] = malloc(argc * sizeof(t_atom));
		}
	}
	x->user_inlet_argcs[idx] = argc;
	memcpy(x->user_inlet_argvs[idx], argv, argc * sizeof(t_atom));
}

void ovm_float(ovm *x, double f)
{
	t_atom a;
	atom_setfloat(&a, f);
	ovm_list(x, NULL, 1, &a);
}

void ovm_int(ovm *x, long l)
{
	t_atom a;
	atom_setlong(&a, l);
	ovm_list(x, NULL, 1, &a);
}

void ovm_assist(ovm *x, void *b, long m, long a, char *s)
{
	if(m == ASSIST_OUTLET){
		long z = x->num_vm_outlets;
		if(a < z){
			switch(a){
			case OVM_OUTLET_OUTPUT:
				sprintf(s, "VM: Output");
				break;
			case OVM_OUTLET_STACK:
				sprintf(s, "VM: Stack");
				break;
			case OVM_OUTLET_ENV:
				sprintf(s, "VM: Environment");
				break;
			}
			return;
		}
		if(x->num_route_outlets){
			z += x->num_route_outlets;
			if(a < z){
				sprintf(s, "Route: Messages that match %s",
					x->route_addrs[a - x->num_vm_outlets]->s_name);
				return;
			}
			z++;
			if(a < z){
				sprintf(s, "Delegation: Messages that did not match any of the route addresses");
				return;
			}
		}
		if(x->prefix){
			z++;
			if(a < z){
				sprintf(s, "Delegation: Messages that do not match the prefix %s\n", x->prefix);
			}
		}
	}else{
		switch(a){
		case OVM_INLET_INPUT:
			sprintf(s, "Input (OSC bundle)");
			break;
		case OVM_INLET_STACK:
			sprintf(s, "Stack (OSC bundle)");
			break;
		case OVM_INLET_ENV:
			sprintf(s, "Environment (OSC bundle)");
			break;
		default:
			sprintf(s, "Arguments for address %s",
				x->user_inlet_addrs[a - x->num_vm_inlets]->s_name);
			break;
		}
	}
}

void ovm_free(ovm *x)
{
	if(x->proxy){
		for(int i = 1; i < x->total_num_inlets; i++){
			if(x->proxy[i]){
				object_free(x->proxy[i]);
			}
		}
		free(x->proxy);
	}
	for(int i = 0; i < x->num_user_inlets; i++){
		if(x->user_inlet_argvs[i]){
			free(x->user_inlet_argvs[i]);
		}
	}
	if(x->user_inlet_addrs){
		free(x->user_inlet_addrs);
	}
	if(x->user_inlet_argcs){
		free(x->user_inlet_argcs);
	}
	if(x->user_inlet_argvs){
		free(x->user_inlet_argvs);
	}
	if(x->route_addrs){
		free(x->route_addrs);
	}
	if(x->route_outlets){
		free(x->route_outlets);
	}
	if(x->vm_outlets){
		free(x->vm_outlets);
	}
	if(x->bytes){
		free(x->bytes);
	}
	critical_free(x->lock);
}

void *ovm_new(t_symbol *sym, long argc, t_atom *argv)
{
	ovm *x = (ovm *)object_alloc(ovm_class);
	if(!x){
		return NULL;
	}
	t_dictionary *d = dictionary_new();
	attr_args_dictionary(d, argc, argv);
	
	x->num_vm_inlets = 3;
	x->num_user_inlets = 0;

	/* can't just call dictionary_getkeys and iterate over that */
	/* because it doesn't include keys for attributes that don't */
	/* have a value */
	for(int i = 0; i < argc; i++){
		if(atom_gettype(argv + i) != A_SYM
		   || atom_getsym(argv + i)->s_name[0] != '@'){
			continue;
		}
		t_symbol *key = gensym(atom_getsym(argv + i)->s_name + 1);
		if(!strncmp("/inlet/", key->s_name, strlen("/inlet/"))){
			x->num_user_inlets++;
		}
	}
	
	x->total_num_inlets = x->num_vm_inlets + x->num_user_inlets;
	x->user_inlet_addrs = (t_symbol **)malloc(x->num_user_inlets * sizeof(t_symbol*));
	x->user_inlet_argcs = (long *)malloc(x->num_user_inlets * sizeof(long));
	x->user_inlet_argvs = (t_atom **)malloc(x->num_user_inlets * sizeof(t_atom*));

	int ic = 0;
	for(int i = 0; i < argc; i++){
		if(atom_gettype(argv + i) != A_SYM
		   || atom_getsym(argv + i)->s_name[0] != '@'){
			continue;
		}
		t_symbol *key = gensym(atom_getsym(argv + i)->s_name + 1);
		if(!strncmp("/inlet/", key->s_name, strlen("/inlet/"))){
			x->user_inlet_addrs[ic] = key;
			long ac = 0;
			t_atom *av = NULL;
			dictionary_getatoms(d, key, &ac, &av);
			x->user_inlet_argcs[ic] = ac;
			x->user_inlet_argvs[ic] = NULL;
			if(ac){
				x->user_inlet_argvs[ic] = (t_atom *)malloc(ac * sizeof(t_atom));
				memcpy(x->user_inlet_argvs[ic], av, ac * sizeof(t_atom));
				
			}
			ic++;
		}
	}

	x->num_route_outlets = 0;
	if(dictionary_hasentry(d, gensym("/outlets"))){
		long ac = 0;
		t_atom *av;
		dictionary_getatoms(d, gensym("/outlets"), &ac, &av);
		x->num_route_outlets = ac;
		x->route_addrs = (t_symbol **)malloc(x->num_route_outlets * sizeof(t_symbol*));
		for(int i = 0; i < ac; i++){
			t_atom *a = av + i;
			if(atom_gettype(a) != A_SYM){
				object_error((t_object *)x,
					     "arguments to attribute /outlets must be symbols");
				ovm_free(x);
				return NULL;
			}
			x->route_addrs[i] = atom_getsym(a);
		}
	}
	
	// make inlets
	x->proxy = (void **)malloc(x->total_num_inlets * sizeof(void *));
	if(!x->proxy){
		object_error((t_object *)x,
			     "couldn't allocate inlet proxies");
		return NULL;
	}
	for(int i = 1; i < x->total_num_inlets; i++){
		x->proxy[i] = proxy_new((t_object *)x,
					x->total_num_inlets - i,
					&(x->inlet));
		if(!x->proxy[i]){
			object_error((t_object *)x,
				     "couldn't allocate inlet proxy");
			return NULL;
		}
	}

	// prefix delegation outlet (if we have a prefix)
	if(dictionary_hasentry(d, gensym("/prefix"))){
		t_symbol *pfx = NULL;
		dictionary_getsym(d, gensym("/prefix"), &pfx);
		x->prefix = pfx->s_name;
		x->prefix_delegation_outlet = outlet_new(x, "FullPacket");
		if(!x->prefix_delegation_outlet){
			object_error((t_object *)x,
				     "couldn't allocate prefix delegation outlet");
			return NULL;		
		}
	}else{
		x->prefix = NULL;
		x->prefix_delegation_outlet = NULL;
	}

	// route outlets (if we have route addresses)
	if(x->num_route_outlets){
		x->route_delegation_outlet = outlet_new(x, "FullPacket");
		if(!x->route_delegation_outlet){
			object_error((t_object *)x,
				     "couldn't allocate route delegation outlet");
			return NULL;		
		}
		x->route_outlets = (void **)malloc(x->num_route_outlets * sizeof(void *));
		if(!x->route_outlets){
			object_error((t_object *)x,
				     "couldn't allocate space for route outlets");
			return NULL;
		}
		for(int i = x->num_route_outlets - 1; i >= 0; i--){
			x->route_outlets[i] = outlet_new(x, NULL);
			if(!x->route_outlets[i]){
				object_error((t_object *)x,
					     "couldn't allocate route outlet");
				return NULL;
			}
		}
	}else{
		x->route_delegation_outlet = NULL;
		x->route_outlets = NULL;
	}

	x->num_vm_outlets = 3;
	// vm outlets
	x->vm_outlets = (void **)malloc(x->num_vm_outlets * sizeof(void *));
	if(!x->vm_outlets){
		object_error((t_object *)x,
			     "couldn't allocate space for vm outlets");
		return NULL;
	}
	for(int i = x->num_vm_outlets - 1; i >= 0; i--){
		x->vm_outlets[i] = outlet_new(x, NULL);
		if(!x->vm_outlets[i]){
			object_error((t_object *)x,
				     "couldn't allocate vm outlet");
			return NULL;
		}
	}
	
	x->bytes = (char *)malloc(65536);
	if(!x->bytes){
		object_error((t_object *)x,
			     "couldn't allocate %d bytes for ose vm",
			     65536);
		return NULL;
	}
	x->bundle = ose_newBundleFromCBytes(65536, x->bytes);
	x->osevm = osevm_init(x->bundle);
	x->vm_i = OSEVM_INPUT(x->osevm);
	x->vm_s = OSEVM_STACK(x->osevm);
	x->vm_e = OSEVM_ENV(x->osevm);
	x->vm_c = OSEVM_CONTROL(x->osevm);
	x->vm_d = OSEVM_DUMP(x->osevm);
	x->vm_o = OSEVM_OUTPUT(x->osevm);
	ose_pushContextMessage(x->osevm, 8192, "/_g");
	x->vm_g = ose_enter(x->osevm, "/_g");

	object_free(d);

	critical_new(&(x->lock));

	return x;
}

void ext_main(void *r)
{
	t_class *c;

	c = class_new("o.se", (method)ovm_new, (method)ovm_free,
		      sizeof(ovm), 0L, A_GIMME, 0);

	class_addmethod(c, (method)ovm_FullPacket, "FullPacket",
			A_LONG, A_LONG, 0);
	class_addmethod(c, (method)ovm_bang, "bang", 0);
	class_addmethod(c, (method)ovm_anything, "anything", A_GIMME, 0);
	class_addmethod(c, (method)ovm_list, "list", A_GIMME, 0);
	class_addmethod(c, (method)ovm_float, "float", A_FLOAT, 0);
	class_addmethod(c, (method)ovm_int, "int", A_LONG, 0);
	class_addmethod(c, (method)ovm_assist, "assist", A_CANT, 0);

	class_register(CLASS_BOX, c);
	ovm_class = c;

	ps_FullPacket = gensym("FullPacket");
}
