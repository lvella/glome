import sys
import mmap
import struct
from gi.repository import Gtk

if len(sys.argv) < 2:
    print('Must supply the entry point file.')
    sys.exit(1)

with open(sys.argv[1], 'r+b') as file:
    mapped = mmap.mmap(file.fileno(), 0)

attributes = (('Scale:', 'f'),
              # --
              ('Maximum rotation per frame:', 'f'),
              ('Maximum speed forward:', 'f'),
              ('Maximum acceleration forward:', 'f'),
              ('Maximum vertical speed:', 'f'),
              ('Maximum horizontal speed:', 'f'),
              ('Maximum spin speed:', 'f'),
              # --
              ('Shot speed:', 'f'),
              ('Shot power:', 'i'),
              ('Canon cooldown rate:', 'i'),
              ('Maximum fire rate:', 'i'))

window = Gtk.Window(title='Ship Stats')
window.connect('delete-event', Gtk.main_quit)

table = Gtk.Table(len(attributes), 2, False)

(_, types) = zip(*attributes)
type_fmt = ''.join(types)
values = struct.unpack(type_fmt, mapped)

def update(adj, data):
    i, data_type = data
    value = adj.get_value()
    if data_type == 'i':
        value = int(value)
    mapped[i*4:(i+1)*4] = struct.pack(data_type, value)

for i in range(len(attributes)):
    label_text, data_type = attributes[i]

    label = Gtk.Label(label_text)
    table.attach(label, 0, 1, i, i+1)

    adj = Gtk.Adjustment(values[i], -1000, 1000, 0.01, 0.1, 0)
    adj.connect('value-changed', update, (i, data_type))

    spin = Gtk.SpinButton()
    spin.set_digits(5)
    spin.set_adjustment(adj)
    table.attach(spin, 1, 2, i, i+1)

window.add(table)
window.show_all()

Gtk.main()
