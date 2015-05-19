import struct
from pylab import *
import matplotlib.animation as animation
import subprocess
import os

class scope():

    def __init__(self):
        cmd = [
            # start gdb and connect
            "arm-none-eabi-gdb",
            "-ex \"target remote localhost:3333\"",
            "-ex \"set confirm off\"",
            "-ex \"file ./adc_utest.elf\"",
            "-ex \"mon reset halt\"",
            "-ex \"tbreak main\"",
            "-ex \"continue\"",

            # quit
            "-ex \"quit\"",
        ]
        subprocess.call(' '.join(cmd), shell=True)
        self.filename = 'adc_cache.bin'
        self.fs = 6e6 / 120
        self.ts = 1 / self.fs
        self.fig = figure()
        self.ax = self.fig.add_subplot(111)
        self.ax.set_xlim([0, 4096*self.ts])
        self.ax.set_ylim([0, 70000])
        self.traces, = self.ax.plot([], [], '-b')
        self.anim = animation.FuncAnimation(self.fig, self.animate, interval=1e1, repeat=True, blit=True)

    def getADCSamples(self, count=4096, fmt="H"):
        cmd = [
            # start gdb and connect
            "arm-none-eabi-gdb",
            "-ex \"target remote localhost:3333\"",
            "-ex \"set confirm off\"",
            "-ex \"delete\"",
            "-ex \"file ./adc_utest.elf\"",
            "-ex \"tbreak adc_handle_buffer\"",
            "-ex \"continue\"",
            # dump adc sample buffer to filename
            "-ex \"dump binary memory %s &buf[0] (&buf[%d])\"" % (self.filename, count),
            # quit
            "-ex \"quit\"",
        ]
        subprocess.call(' '.join(cmd), shell=True)
        buf = open(self.filename).read()
        size = struct.Struct(fmt).size
        u = struct.unpack("<%d" % (len(buf)/size) + fmt, buf)
        return array(list(u))

    def animate(self, n):
        samples = self.getADCSamples()
        t = arange(0, (len(samples))*self.ts, self.ts)
        self.traces.set_data(t, samples)
        self.fig.canvas.draw()
        return self.traces,
        
if __name__ == "__main__":
    s = scope()
    show()
