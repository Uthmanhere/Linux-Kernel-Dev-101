# Direct Memory Access

## Modes

- Burst
- Cycles Stealing
- Transparent

## Memory Mappings

- Fully Coherent
- Write Combining
- Weakly Ordered
- Non-Coherent

## Buffers

- Contiguous
- Scatter/Gather
- Bounce

## ~~Cache Management~~

## DMA Transfer

1. Hardware Interrupt
2. Interrupt Handler: allocates buffer and returns the address
3. Device: writes data and raises the interrupt when done.
4. Interrupt Handler: awakens the relevant process

## DMA Mapping

1. Allocate Memory for DMA.
2. Map DMA memory to dev.
3. Map DMA memory to userspace.
4. Synchronize memory between CPU & dev.

### Tips

- Use correct API.
	- Coherent Mappings
	- Streaming Mappings
- Set DMA masks
- Use `dma_mapping_error()`

