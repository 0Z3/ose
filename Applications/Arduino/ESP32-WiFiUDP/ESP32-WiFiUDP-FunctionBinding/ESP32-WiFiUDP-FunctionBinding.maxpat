{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 1,
			"revision" : 1,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 499.0, 79.0, 946.0, 479.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 1,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 1,
		"objectsnaponopen" : 1,
		"statusbarvisible" : 2,
		"toolbarvisible" : 1,
		"lefttoolbarpinned" : 0,
		"toptoolbarpinned" : 0,
		"righttoolbarpinned" : 0,
		"bottomtoolbarpinned" : 0,
		"toolbars_unpinned_last_save" : 0,
		"tallnewobj" : 0,
		"boxanimatetime" : 200,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"style" : "",
		"subpatcher_template" : "",
		"boxes" : [ 			{
				"box" : 				{
					"id" : "obj-20",
					"linecount" : 5,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 289.0, 20.5, 458.0, 66.0 ],
					"presentation_linecount" : 5,
					"text" : "1. Push a uniform random value between [0,1] onto the stack and multiply it by 1000.\n2. Push a string \"/r\" onto the stack, and then onto the end of the value produced by the first message, then move it from the data section to the address field.\n3. Duplicate the message on the stack, and move the copy to the output bundle.\n4. Sleep for the duration that's on the stack (in milliseconds)."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-18",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 148.0, 372.0, 372.0, 30.0 ],
					"presentation_linecount" : 4,
					"text" : "o.downcast is necessary here, because our sketch only supports 32-bit floats, and odot's default floating point type is 64-bits (double)."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-16",
					"linecount" : 5,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 451.0, 283.5, 458.0, 66.0 ],
					"presentation_linecount" : 13,
					"text" : "1. Push a uniform random value between [0,1] onto the stack and multiply it by 1000.\n2. Push a string \"/r\" onto the stack, and then onto the end of the value produced by the first message, then move it from the data section to the address field.\n3. Duplicate the message on the stack, and move the copy to the output bundle.\n4. Sleep for the duration that's on the stack (in milliseconds)."
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontsize" : 12.0,
					"id" : "obj-12",
					"linecount" : 4,
					"maxclass" : "o.compose",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 75.0, 283.5, 366.0, 65.0 ],
					"saved_bundle_data" : [ 35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 47, 33, 47, 109, 117, 108, 0, 0, 44, 115, 100, 0, 47, 33, 47, 117, 110, 105, 102, 111, 114, 109, 0, 0, 64, -113, 64, 0, 0, 0, 0, 0, 0, 0, 0, 44, 47, 33, 47, 115, 116, 114, 105, 110, 103, 47, 116, 111, 97, 100, 100, 114, 101, 115, 115, 47, 109, 111, 118, 101, 0, 0, 0, 0, 44, 115, 115, 0, 47, 114, 0, 0, 47, 33, 47, 112, 117, 115, 104, 0, 0, 0, 0, 16, 47, 45, 0, 0, 44, 115, 0, 0, 47, 33, 47, 100, 117, 112, 0, 0, 0, 0, 0, 16, 47, 33, 47, 115, 108, 101, 101, 112, 0, 0, 0, 0, 44, 0, 0, 0 ],
					"saved_bundle_length" : 140,
					"text" : "/!/mul : [\"/!/uniform\", 1000.],\n/!/string/toaddress/move : [\"/r\", \"/!/push\"],\n/- : \"/!/dup\",\n/!/sleep"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-6",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 75.0, 377.0, 69.0, 20.0 ],
					"text" : "o.downcast"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-11",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 216.0, 222.0, 210.0, 18.0 ],
					"text" : "Get a random number between [0,1]"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-10",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 216.0, 189.0, 193.0, 30.0 ],
					"text" : "Toggle the onboard LED (without an argument, this will flip the state)"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-9",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 109.0, 153.0, 150.0, 18.0 ],
					"text" : "Toggle the onboard LED"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontsize" : 12.0,
					"id" : "obj-78",
					"maxclass" : "o.compose",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 32.0, 189.0, 150.0, 24.0 ],
					"saved_bundle_data" : [ 35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 47, 33, 47, 108, 101, 100, 0, 0, 44, 0, 0, 0 ],
					"saved_bundle_length" : 32,
					"text" : "/!/led"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-40",
					"maxclass" : "toggle",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 18.0, 110.0, 24.0, 24.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-38",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 18.0, 153.0, 83.0, 20.0 ],
					"text" : "o.pack /!/led 0"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontsize" : 12.0,
					"id" : "obj-36",
					"maxclass" : "o.compose",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 47.0, 222.0, 150.0, 24.0 ],
					"saved_bundle_data" : [ 35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 47, 45, 0, 0, 44, 115, 0, 0, 47, 33, 47, 117, 110, 105, 102, 111, 114, 109, 0, 0 ],
					"saved_bundle_length" : 40,
					"text" : "/- : \"/!/uniform\""
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontsize" : 12.0,
					"id" : "obj-4",
					"maxclass" : "o.display",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 438.0, 141.0, 191.0, 34.0 ],
					"text" : "/r : 447.759"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-2",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 438.0, 110.0, 149.0, 20.0 ],
					"text" : "udpreceive 10001 CNMAT"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 18.0, 427.0, 178.0, 20.0 ],
					"text" : "udpsend 192.168.178.56 10000"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 0 ],
					"source" : [ "obj-12", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4", 0 ],
					"source" : [ "obj-2", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-36", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-38", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-38", 0 ],
					"source" : [ "obj-40", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-6", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-78", 0 ]
				}

			}
 ],
		"dependency_cache" : [ 			{
				"name" : "o.display.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "o.compose.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "o.pack.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "o.downcast.mxo",
				"type" : "iLaX"
			}
 ],
		"autosave" : 0
	}

}
