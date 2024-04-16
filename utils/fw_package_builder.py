import os
import sys
from intelhex import IntelHex

def main(hex_file1, hex_file2, hex_merged):
    ih1 = IntelHex()
    ih2 = IntelHex()
    ih_gap = IntelHex()
    ih_merged = IntelHex()
    
    # Remove the existent output file
    if( os.path.exists(hex_merged) ):
        os.remove( hex_merged )
    
    ih1.fromfile(hex_file1,format='hex')
    ih2.fromfile(hex_file2,format='hex')

    # Ensure the sequence of files
    if( ih1.minaddr() > ih2.minaddr() ):
        ih_temp = ih1
        ih1 = ih2
        ih2 = ih_temp
        
    
    # Prevent the overlap
    if( ih1.maxaddr() >= ih2.minaddr() ):
        sys.exit( "The files are overlapping each other." )

    # Fill the gap between the files
    for addr in range( ih1.maxaddr() + 1, ih2.minaddr() ):
        ih_gap[addr] = 0xFF

    ih_merged.merge(ih1, overlap='replace')
    ih_merged.merge(ih_gap, overlap='replace')
    ih_merged.merge(ih2, overlap='replace')

    ih_merged.write_hex_file(hex_merged)

if __name__ == "__main__":
    hex_file1 = sys.argv[1]
    hex_file2 = sys.argv[2]
    hex_merged = sys.argv[3]
    main(hex_file1, hex_file2, hex_merged)