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
		"rect" : [ 34.0, 79.0, 1657.0, 1018.0 ],
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
					"id" : "obj-57",
					"linecount" : 9,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 1087.5, 786.0, 489.0, 114.0 ],
					"presentation_linecount" : 10,
					"text" : "call our uniform function which puts a value between [0,1] on the stack\npush the value 1000. on the stack\nmultiply them together\npush the string /r on the stack\npush the string /r onto the end of the message that contains the results of the multiplication\nmove the string /r from the end of the message to the address\nduplicate the message on the stack\nmove the copy we just made to the output bundle\ncall our sleep function, which calls arduino's delay() function"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-56",
					"linecount" : 8,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 999.5, 406.0, 289.0, 102.0 ],
					"presentation_linecount" : 8,
					"text" : "\n\nlookup the value of pin 21 and put it on the stack\nassign the value on the stack to /r in the environment\nlookup /r and put it on the stack\nassign the value on the stack to pin 13\nlookup /r again and put it on the stack\nmove the value on the stack to the output bundle"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-53",
					"linecount" : 3,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 1215.25, 629.5, 233.5, 42.0 ],
					"presentation_linecount" : 4,
					"text" : "This calls the /ping function we implemented which will respond on port 1000 with the local address on the device:"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-52",
					"linecount" : 4,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 567.0, 724.0, 372.0, 54.0 ],
					"presentation_linecount" : 4,
					"text" : "This example chooses a random number and multiplies it by 1000 to get a duration in miliseconds. It then puts a copy of that value in the output bundle, and sleeps for that duration, sending the output bundle back to use after the duration has elapsed."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-51",
					"linecount" : 8,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 943.5, 406.0, 345.0, 102.0 ],
					"presentation_linecount" : 8,
					"text" : "Note, the address is the last part of a message to be evaluated.\n\n/$/d/21\n/@/r \n/$/r\n/@/d/13 \n/$/r \n/- "
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-50",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 473.0, 298.0, 491.0, 30.0 ],
					"presentation_linecount" : 6,
					"text" : "These two examples read a value from one pin, and assign it to another. For example, you might connect a button to pin 21, which will cause it to turn the LED on pin 13 on and off."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-49",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 423.0, 87.0, 207.0, 18.0 ],
					"text" : "Other examples of the \"default\" hook:"
				}

			}
, 			{
				"box" : 				{
					"bubble" : 1,
					"bubbleside" : 3,
					"id" : "obj-47",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 14.0, 730.5, 35.0, 22.0 ],
					"text" : "4"
				}

			}
, 			{
				"box" : 				{
					"bubble" : 1,
					"bubbleside" : 3,
					"id" : "obj-46",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 14.0, 597.5, 35.0, 22.0 ],
					"text" : "3"
				}

			}
, 			{
				"box" : 				{
					"bubble" : 1,
					"bubbleside" : 3,
					"id" : "obj-45",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 14.0, 373.0, 35.0, 22.0 ],
					"text" : "2"
				}

			}
, 			{
				"box" : 				{
					"bubble" : 1,
					"bubbleside" : 3,
					"id" : "obj-44",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 14.0, 124.0, 35.0, 22.0 ],
					"text" : "1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-41",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 146.0, 694.5, 264.0, 30.0 ],
					"presentation_linecount" : 2,
					"text" : "Without a value, our implementation of the /led function toggles the state of the LED."
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontsize" : 12.0,
					"id" : "obj-39",
					"maxclass" : "o.compose",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 146.0, 730.5, 64.0, 24.0 ],
					"saved_bundle_data" : [ 35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 47, 33, 47, 108, 101, 100, 0, 0, 44, 0, 0, 0 ],
					"saved_bundle_length" : 32,
					"text" : "/!/led"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-37",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 126.0, 561.5, 264.0, 30.0 ],
					"presentation_linecount" : 2,
					"text" : "This calls the function \"/led\" that we installed in the environment. "
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-35",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 94.0, 336.0, 262.0, 30.0 ],
					"presentation_linecount" : 2,
					"text" : "We overloaded Ose's assignment address (/@) to mean \"assign a value to a pin."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-34",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 58.0, 87.0, 331.0, 30.0 ],
					"presentation_linecount" : 2,
					"text" : "This makes use of our implementation of the \"default\" hook, for processing address strings that Ose doesn't respond to."
				}

			}
, 			{
				"box" : 				{
					"bubblepoint" : 0.0,
					"bubbleside" : 2,
					"id" : "obj-33",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 14.0, 42.0, 378.0, 18.0 ],
					"text" : "Four ways of toggling the onboard LED (pin 13 on Adafruit's Feather):"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontsize" : 12.0,
					"id" : "obj-19",
					"maxclass" : "o.compose",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1215.25, 676.0, 150.0, 24.0 ],
					"saved_bundle_data" : [ 35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 47, 33, 47, 112, 105, 110, 103, 0, 44, 0, 0, 0 ],
					"saved_bundle_length" : 32,
					"text" : "/!/ping"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-20",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 1215.25, 713.0, 185.0, 20.0 ],
					"text" : "udpsend 192.168.178.255 10000"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-21",
					"linecount" : 3,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 582.0, 904.5, 351.0, 42.0 ],
					"presentation_linecount" : 3,
					"text" : "Note: o.downcast is necessary here, because our sketch only supports 32-bit floats, and odot's default floating point type is 64-bits (double)."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-22",
					"linecount" : 9,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 943.0, 786.0, 137.0, 114.0 ],
					"presentation_linecount" : 9,
					"text" : "/!/uniform\n1000.\n/!/mul\n/r\n/!/push\n/!/string/toaddress/move\n/!/dup\n/-\n/!/sleep"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontsize" : 12.0,
					"id" : "obj-23",
					"linecount" : 4,
					"maxclass" : "o.compose",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 567.0, 786.0, 366.0, 65.0 ],
					"presentation_linecount" : 4,
					"saved_bundle_data" : [ 35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 47, 33, 47, 109, 117, 108, 0, 0, 44, 115, 100, 0, 47, 33, 47, 117, 110, 105, 102, 111, 114, 109, 0, 0, 64, -113, 64, 0, 0, 0, 0, 0, 0, 0, 0, 44, 47, 33, 47, 115, 116, 114, 105, 110, 103, 47, 116, 111, 97, 100, 100, 114, 101, 115, 115, 47, 109, 111, 118, 101, 0, 0, 0, 0, 44, 115, 115, 0, 47, 114, 0, 0, 47, 33, 47, 112, 117, 115, 104, 0, 0, 0, 0, 16, 47, 45, 0, 0, 44, 115, 0, 0, 47, 33, 47, 100, 117, 112, 0, 0, 0, 0, 0, 16, 47, 33, 47, 115, 108, 101, 101, 112, 0, 0, 0, 0, 44, 0, 0, 0 ],
					"saved_bundle_length" : 140,
					"text" : "/!/mul : [\"/!/uniform\", 1000.],\n/!/string/toaddress/move : [\"/r\", \"/!/push\"],\n/- : \"/!/dup\",\n/!/sleep"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-24",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 567.0, 879.5, 69.0, 20.0 ],
					"text" : "o.downcast"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-25",
					"linecount" : 4,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 691.0, 629.5, 462.0, 54.0 ],
					"presentation_linecount" : 10,
					"text" : "This calls the function \"/uniform\" that we implemented that puts a uniform random value between [0,1], generated by the ESP32's true random number generator, on the stack. The address \"/-\" transfers the message from the stack to the output bundle, which is automatically sent back to us when the bundle has been fully processed."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-29",
					"maxclass" : "toggle",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 126.0, 596.5, 24.0, 24.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-30",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 126.0, 629.5, 83.0, 20.0 ],
					"text" : "o.pack /!/led 0"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontsize" : 12.0,
					"id" : "obj-31",
					"maxclass" : "o.compose",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 533.0, 629.5, 150.0, 24.0 ],
					"saved_bundle_data" : [ 35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 47, 45, 0, 0, 44, 115, 0, 0, 47, 33, 47, 117, 110, 105, 102, 111, 114, 109, 0, 0 ],
					"saved_bundle_length" : 40,
					"text" : "/- : \"/!/uniform\""
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-18",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 563.0, 391.0, 202.0, 30.0 ],
					"presentation_linecount" : 2,
					"text" : "Set the value of digital pin 13 (onboard LED) to the value of pin 21"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-8",
					"maxclass" : "toggle",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 792.5, 336.0, 24.0, 24.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-13",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 792.5, 367.0, 56.0, 20.0 ],
					"text" : "metro 20"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-14",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 943.5, 372.0, 345.0, 30.0 ],
					"text" : "This also reads the value of pin 21 and assigns it to pin 13, but it also puts a copy in the output bundle."
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontsize" : 12.0,
					"id" : "obj-72",
					"linecount" : 3,
					"maxclass" : "o.compose",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 792.5, 391.0, 143.0, 51.0 ],
					"saved_bundle_data" : [ 35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 47, 64, 47, 114, 0, 0, 0, 0, 44, 115, 0, 0, 47, 36, 47, 100, 47, 50, 49, 0, 0, 0, 0, 20, 47, 64, 47, 100, 47, 49, 51, 0, 44, 115, 0, 0, 47, 36, 47, 114, 0, 0, 0, 0, 0, 0, 0, 16, 47, 45, 0, 0, 44, 115, 0, 0, 47, 36, 47, 114, 0, 0, 0, 0 ],
					"saved_bundle_length" : 84,
					"text" : "/@/r : \"/$/d/21\",\n/@/d/13 : \"/$/r\",\n/- : \"/$/r\""
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-16",
					"maxclass" : "toggle",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 94.0, 372.0, 24.0, 24.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-17",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 94.0, 403.0, 99.0, 20.0 ],
					"text" : "o.pack /@/d/13 1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-12",
					"maxclass" : "toggle",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 473.0, 336.0, 24.0, 24.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-7",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 473.0, 367.0, 56.0, 20.0 ],
					"text" : "metro 20"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontsize" : 12.0,
					"id" : "obj-6",
					"linecount" : 2,
					"maxclass" : "o.compose",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 473.0, 391.0, 78.0, 38.0 ],
					"saved_bundle_data" : [ 35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 47, 36, 47, 100, 47, 50, 49, 0, 44, 0, 0, 0, 0, 0, 0, 12, 47, 64, 47, 100, 47, 49, 51, 0, 44, 0, 0, 0 ],
					"saved_bundle_length" : 48,
					"text" : "/$/d/21,\n/@/d/13"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-5",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 688.0, 109.0, 189.0, 30.0 ],
					"text" : "Set the value of all the digital pins to 1 (HIGH), and get their values"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontsize" : 12.0,
					"id" : "obj-3",
					"linecount" : 2,
					"maxclass" : "o.compose",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 688.0, 142.0, 85.0, 38.0 ],
					"saved_bundle_data" : [ 35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 47, 100, 47, 42, 0, 0, 0, 0, 44, 105, 0, 0, 0, 0, 0, 1, 0, 0, 0, 12, 47, 100, 47, 42, 0, 0, 0, 0, 44, 0, 0, 0 ],
					"saved_bundle_length" : 52,
					"text" : "/d/* : 1,\n/d/*"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-11",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 544.0, 109.0, 125.0, 30.0 ],
					"text" : "Get the value of digital pins 13 and 21"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-10",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 423.0, 109.0, 105.0, 30.0 ],
					"text" : "Get the value of all the analog pins"
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
					"patching_rect" : [ 423.0, 142.0, 49.0, 24.0 ],
					"saved_bundle_data" : [ 35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 47, 97, 47, 42, 0, 0, 0, 0, 44, 0, 0, 0 ],
					"saved_bundle_length" : 32,
					"text" : "/a/*"
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
					"patching_rect" : [ 58.0, 124.0, 24.0, 24.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-38",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 58.0, 153.0, 84.0, 20.0 ],
					"text" : "o.pack /d/13 1"
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
					"patching_rect" : [ 544.0, 142.0, 93.0, 24.0 ],
					"saved_bundle_data" : [ 35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 47, 100, 47, 123, 49, 51, 44, 50, 49, 125, 0, 0, 44, 0, 0, 0 ],
					"saved_bundle_length" : 36,
					"text" : "/d/{13,21}"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontsize" : 12.0,
					"id" : "obj-4",
					"linecount" : 2,
					"maxclass" : "o.display",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1400.0, 63.0, 191.0, 48.0 ],
					"text" : "/d/21 : 0,\n/d/13 : 0"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-2",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1400.0, 32.0, 149.0, 20.0 ],
					"text" : "udpreceive 10001 CNMAT"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 58.0, 935.0, 178.0, 20.0 ],
					"text" : "udpsend 192.168.178.56 10000"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-7", 0 ],
					"source" : [ "obj-12", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-72", 0 ],
					"source" : [ "obj-13", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"source" : [ "obj-16", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-17", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-20", 0 ],
					"source" : [ "obj-19", 0 ]
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
					"destination" : [ "obj-24", 0 ],
					"source" : [ "obj-23", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 576.5, 922.0, 67.5, 922.0 ],
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-30", 0 ],
					"source" : [ "obj-29", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 697.5, 201.0, 431.0, 201.0, 431.0, 921.0, 67.5, 921.0 ],
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-30", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 542.5, 921.0, 67.5, 921.0 ],
					"source" : [ "obj-31", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 553.5, 184.0, 432.0, 184.0, 432.0, 921.0, 67.5, 921.0 ],
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
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-39", 0 ]
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
					"midpoints" : [ 482.5, 921.0, 67.5, 921.0 ],
					"source" : [ "obj-6", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 0 ],
					"source" : [ "obj-7", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 802.0, 462.0, 483.0, 462.0, 483.0, 922.0, 67.5, 922.0 ],
					"source" : [ "obj-72", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 432.5, 922.0, 67.5, 922.0 ],
					"source" : [ "obj-78", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 0 ],
					"source" : [ "obj-8", 0 ]
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
