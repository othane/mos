/**
 * @file bootstrap.h
 *
 * @brief interface to the module that deals with starting the bootloader, or application, etc
 *
 * @author OT
 *
 * @date Aprl 01 2013
 *
 */

#ifndef __BOOTSTRAP__
#define __BOOTSTRAP__


// describes a program the bootstrap can run
///@todo this should be stm32f107 specific
#define BOOTSTRAP_PROG_HEADER 0x01
packed_start
packed(struct) bootstrap_prog_header
{
	uint32_t crc;         // crc32 over header and program minus the crc word
	uint32_t len;         // size of this header and program
	uint8_t  type;        // header type, prog_header in this case (this is just a unique code to ensure this header is intended to describe a prog as other headers may exist)
	void	 *isr_vector; // points to the start of the programs isr_vector table
	uint8_t  pid;	      // program id number
};
packed_end


// opaque description of a program the bootstrap can start
typedef struct bootstrap_prog_header bootstrap_prog_header;


// use these to select applications to boot
extern const bootstrap_prog_header *bootstrap_bootloader_header;
extern const bootstrap_prog_header *bootstrap_program_headers[];


/**
 * @brief checks the header to see if it describes a valid program
 * @param header points to a program header for the program to check
 * @return true if len > 0, type == BOOTSTRAP_PROG_HEADER, crc match; otherwise false
 */
bool validate_prog(bootstrap_prog_header *header);


/**
 * @brief reboot into bootstrap and run this program
 * @param header points to a program header for the program to start
 * @return 
 */
bool bootstrap_switch(bootstrap_prog_header *header);


#endif

