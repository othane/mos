import struct
from pylab import *
import matplotlib.animation as animation
import subprocess
import os

def genADCSamples(filename, count=4096):
    cmd = [
        # start gdb and connect
        "arm-none-eabi-gdb",
        #"-x \"debug_adc_utest.gdbinit\"",
        "-x \"run_adc_utest.gdbinit\"",
        # dump adc sample buffer to filename
        "-ex \"dump binary memory %s &buf[0] (&buf[%d])\"" % (filename, count),
        # quit
        "-ex \"quit\"",
    ]
    subprocess.call(' '.join(cmd), shell=True)

def loadADCSamples(filename, fmt="H"):
    buf = open(filename).read()
    size = struct.Struct(fmt).size
    u = struct.unpack("<%d" % (len(buf)/size) + fmt, buf)
    return array(list(u))

def showADCSamples(buf, scale=3.3/65536):
    fs = 6e6 / 120
    ts = 1 / fs
    t = arange(0, (len(buf))*ts, ts)
    plot(t, buf * scale)
    grid(True)
    ylim([0, 3.5])
    show()

class scope():

    def __init__(self):
        self.filename = 'adc_cache.bin'
        self.fs = 6e6 / 120
        self.ts = 1 / self.fs
        self.fig = figure()
        self.ax = self.fig.add_subplot(111)
        self.ax.set_xlim([0, 4096*self.ts])
        self.ax.set_ylim([0, 65535])
        self.traces, = self.ax.plot([], [], '-b')
        self.anim = animation.FuncAnimation(self.fig, self.animate, interval=1e1, repeat=True, blit=True)

    def animate(self, n):
        genADCSamples(self.filename)
        samples = loadADCSamples(self.filename)
        #samples = rand(4096)
        t = arange(0, (len(samples))*self.ts, self.ts)
        self.traces.set_data(t, samples)
        self.fig.canvas.draw()
        return self.traces,
        
if __name__ == "__main__":
    s = scope()
    show()
