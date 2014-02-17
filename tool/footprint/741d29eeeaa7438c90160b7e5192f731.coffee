#format 1.2
#name spiratronics-DIN5-midi
#id 741d29eeeaa7438c90160b7e5192f731
#parent 36fbee849c59496db8560db10fd5e692
#desc spiratronics DIN5 PCB mount MIDI connector

footprint = () ->

  # total size
  total_x = 19
  left = -total_x/2

  # pads
  drill_r = 1.5/2
  pad_r = drill_r + 0.3

  pad1 = new RoundPad pad_r, drill_r*2
  pad1.x = left + 12.5
  pad1.y = -7.5
  pad1.name = 1

  pad2 = clone pad1
  pad2.y = 0
  pad2.name = 2

  pad3 = mirror_x clone pad1
  pad3.name = 3

  pad4 = clone pad1
  pad4.y = -5
  pad4.x = left + 15
  pad4.name = 4

  pad5 = mirror_x clone pad4
  pad5.name = 5

  pad6 = clone pad4
  pad6.x = left + 3
  pad6.name = 6
  
  pad7 = mirror_x clone pad6
  pad7.name = 7

  pads = [pad1, pad2, pad3, pad4, pad5, pad6, pad7]
 
  name = new Name 12

  total_y = 21
  w = 0.1

  reduced_y = 9
  conn_r = 3.6
  conn_y = 2*conn_r

  r4 = make_rect total_x, total_y, w, 'silk'

  combine [name,pads, r4]
 