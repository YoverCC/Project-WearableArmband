# -*- coding: utf-8 -*-
"""
Created on Mon Dec  2 18:01:36 2019

@author: LIIAARC LAPTOP - 02
"""

import pyqtgraph
import numpy
import threading
import time

class FuncThread(threading.Thread):
 def __init__(self,target,*args):
  self._target=target
  self._args=args
  threading.Thread.__init__(self)
 def run(self):
  self._target(*self._args)

class MainApp(object):
 def __init__(self):
  plot_loop_time = 0.1
  self.sleep_time = 0.001
  self.number_of_points = 100
  self.collected_data = numpy.zeros([self.number_of_points])
  app = pyqtgraph.mkQApp()
  self.graphics_layout_widget = pyqtgraph.GraphicsLayoutWidget()
  self.graphics_layout_widget.show() 
  plot = self.graphics_layout_widget.addPlot()
  plot.setRange(yRange=(-200,200),disableAutoRange=True)
  cd = FuncThread(self.collectData)
  cd.start()
  current_time = time.time()
  while self.graphics_layout_widget.isVisible():
   time.sleep(self.sleep_time)
   plot.plot(self.collected_data)
   app.processEvents()
   while time.time() - current_time < plot_loop_time:
    time.sleep(self.sleep_time)
   plot.clear()
   current_time = time.time()
  
 def collectData(self):
  while self.graphics_layout_widget.isVisible():
   self.collected_data += numpy.random.uniform(-1, 1, size=self.number_of_points)
   time.sleep(self.sleep_time)

MainApp()
