#format 1.2
#name SPC-FD05-3.5mm-stereo-phone-jack
#id 36fbee849c59496db8560db10fd5e692
#parent 356cfdfb4d024f9da597c1d080b56c3e
#desc multicomp SPC24110
#desc Farnell 2112339

footprint = () ->

  # total size
  total_x = 14
  left = -total_x/2

  # alignment/mounting holes
  # use pad for hole to facilitate Kicad/oshpark
  alignment_hole = 0.9
  ah = alignment_hole
  drill1 = new RoundPad ah/2, ah
  drill1.y = 10/2
  drill1.x = left + 6.5-2.5
  drill1.name = 'd1'
  drill2 = mirror_x clone drill1
  drill2.name = 'd2'
  drill3 = clone drill1
  drill3.name = 'd3'
  drill3.x = drill1.x + 2.5
  drill4 = mirror_x clone drill3
  drill4.name = 'd4'
  drill5 = clone drill3
  drill5.x = drill3.x + 5
  drill5.y = 0
  drill5.name = 'd5'
  drills = [drill1, drill2, drill3, drill4, drill5]


  # pads
  drill_r = 1.5/2
  pad_r = drill_r + 0.3

  pad1 = new RoundPad pad_r, drill_r*2
  pad1.x = drill3.x
  pad1.y = 0
  pad1.name = 3

  pad2 = clone pad1
  pad2.x = drill5.x
  pad2.y = 5/2
  pad2.name = 5

  pad3 = mirror_x clone pad2
  pad3.name = 4

  pad4 = clone pad2
  pad4.y += 2.5
  pad4.name = 6

  pad5 = mirror_x clone pad4
  pad5.name = 7

  pads = [pad1, pad2, pad3, pad4, pad5]
 
  name = new Name 8

  total_y = 12
  w = 0.1
  r1 = make_rect total_x, total_y, w, 'docu'

  reduced_y = 9
  conn_r = 3.6
  conn_y = 2*conn_r

  l1 = new Line w, 'silk'
  l1.x1 = left + 2.8
  l1.x2 = l1.x1
  l1.y1 = total_y/2
  l1.y2 = -total_y/2

  r2 = adjust_x (make_rect 1, reduced_y, w, 'docu'), left-0.5+2.8

  r3 = adjust_x (make_rect 1.8, conn_y, w, 'docu'), left-0.9+1.8

  r4 = adjust_x (make_rect total_x-2.8, total_y, w, 'silk'), 2.8/2
  docu = combine [r2,r3]
  silk = combine [l1, r4]

  combine [docu,name,pads, silk, drills]
 