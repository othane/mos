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
#define BOOTSTRAP_PROG_HEADER_STM32F107_HW_ID 1
packed_start
packed(struct) bootstrap_prog_header
{
	uint32_t crc;         /**< crc32 over header and program minus the crc word */
	uint32_t len;         /**< size of this header and program */
	uint8_t  type;        /**< header type, prog_header in this case (this is just a unique code to ensure this header is intended to describe a prog as other headers may exist) */
	void	 *isr_vector; /**< points to the start of the programs isr_vector table */
	uint8_t  pid;	      /**< program id number (1...65536, pid=0 is the bootstrap) */
	uint32_t max_len;	  /**< maximum size reserved for this app */
	uint32_t key;		  /**< key value needed by the bootloader to unlock this program and update it */
	uint16_t hw_id;       /**< hardware id, this is a code to indicate what hardware this program can run on */
};
packed_end


// opaque description of a program the bootstrap can start
typedef struct bootstrap_prog_header bootstrap_prog_header;


/**
 * @brief given a pid try to find the corresponding header
 * @param pid to lookup
 * @return pointer to the header for this program
 */
const struct bootstrap_prog_header * bootstrap_get_program_header(uint8_t pid);


/**
 * @brief set the pid of the program to start on next reboot
 * @param pid program id to start on reboot
 */
void bootstrap_set_boot_pid(uint16_t pid);


/**
 * @brief get the pid of the program to start
 * @return pid program id to start 
 */
uint16_t bootstrap_get_boot_pid(void);


/**
 * @brief checks the header to see if it describes a valid program
 * @param header points to a program header for the program to check
 * @return true if len > 0, type == BOOTSTRAP_PROG_HEADER, crc match; otherwise false
 */
bool bootstrap_validate_prog(const bootstrap_prog_header *header);


/**
 * @brief boot the program described in the program header
 * @param header points to a header describing the program to boot
 */
void boot(const bootstrap_prog_header *header);


#endif

