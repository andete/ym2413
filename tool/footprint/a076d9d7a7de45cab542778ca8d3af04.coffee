#format 1.2
#name TQFP64
#id a076d9d7a7de45cab542778ca8d3af04
#parent d2eee7c9259c4bb9bc62a418ba043dfb
#desc TQFP48 example
#desc based on the EFM32GG332 spec

footprint = () ->

  size = 11.5
  psize = 10
  w = 0.25
  num_pads = 64
  e = 0.5

  name = new Name 1.5
  value = new Value -1.5

  pad = new Smd
  pad.dx = 1.6
  pad.dy = 0.3
  pad.ro = 0

  pads = quad [pad], num_pads, e, size

  docu = make_rect psize-w, psize-w, w/2, 'docu'
  silk = make_rect psize-(3*w), psize-(3*w), w, 'silk'

  dot = new Disc (w*2)
  dot.x = -size/2+0.2
  dot.y = size/2-0.2 

  combine [name, value, pads, silk, dot, docu]