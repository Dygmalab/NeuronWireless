import sys
from intelhex import IntelHex

def main(hex_file1, hex_file2, hex_merged):
    ih1 = IntelHex()
    ih2 = IntelHex()
    ih_gap = IntelHex()
    ih_merged = IntelHex()
    
    ih1.fromfile(hex_file1,format='hex')
    ih2.fromfile(hex_file2,format='hex')

    for addr in range( ih1.maxaddr() + 1, ih2.minaddr() ):
        ih_gap[addr] = 0xFF

    ih_merged.merge(ih1, overlap='replace')
    ih_merged.merge(ih_gap, overlap='replace')
    ih_merged.merge(ih2, overlap='replace')

    ih_merged.write_hex_file(hex_merged)

if __name__ == "__main__":
    hex_file1 = "../libraries/rf_host_device/rf_host_device/bin/rf_host_device.hex"
    hex_file2 = "../build/" + sys.argv[1] + "/Wireless_neuron.hex"
    hex_merged = "Wireless_neuron_packed.hex"
    main(hex_file1, hex_file2, hex_merged)