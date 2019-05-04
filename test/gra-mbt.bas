5 cls() : print "\n\n\n      Mandelbrot Fractal Generation"
10 xmin=-8601
20 xmax=2867
30 ymin=-4915
40 ymax=4915
50 maxiter=64
60 dx=(xmax-xmin)/200
70 dy=(ymax-ymin)/200
80 cy=ymax : yp=0
90 cx=xmin : xp=0
100 iter = 0 : x=0 : y=0 : x2=0 : y2=0
110 y=((x*y)/2048)+cy
120 x=x2-y2+cx
130 x2=(x*x) / 4096
140 y2=(y*y) / 4096
150 if x2 + y2 > 16384 then goto 180
160 iter = iter + 1
170 if iter < maxiter then goto 110
180 DRAWLINE(xp,yp,xp+2,yp,iter*4)
185 print "xp:",xp," yp:",yp
200 cx = cx + dx : xp = xp + 2
210 if cx < xmax then goto 100
220 cy = cy - dy : yp = yp + 1
230 if cy > ymin then goto 90
