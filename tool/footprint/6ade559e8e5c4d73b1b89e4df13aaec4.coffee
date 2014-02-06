#format 1.2
#name CRYSTAL-TXC-8Z-2.5x2
#id 6ade559e8e5c4d73b1b89e4df13aaec4
#parent 4dcef9c90bb24a8eb27fdc201175a87b
footprint = () ->

  tdx = 2.5
  tdy = 2
  
  smd = new Smd
  smd.dx = 0.9
  smd.dy = 1

  e = 0.85+smd.dy
  between = 0.65+smd.dx

  l = rot_dual smd, 4, e, between

  rx = smd.dy+e+0.2
  ry = smd.dx+between+0.2

  rd = make_rect tdx, tdy, 0.1, 'docu'
  rs = make_rect rx, ry, 0.1, 'silk'

  rx1 = make_rect smd.dy+0.3, smd.dx+0.3, 0.1, 'silk'
  rx1 = adjust_x rx1, -rx/4-smd.dy/4+0.15
  rx1 = adjust_y rx1, -ry/4-smd.dx/4+0.15
  rx2 = mirror_x mirror_y clone rx1

  name = new Name 3
  value = new Value -3

  combine [l, name, value, rd, rs, rx1, rx2]