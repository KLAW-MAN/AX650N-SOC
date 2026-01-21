#ifndef _AXERA_UPDATE_H_
#define _AXERA_UPDATE_H_

#define VERSION_2_PARSE_XML

#ifdef OPTEE_BOOT
#define XML_NAME     "AX650X_optee.xml"
#else
#define XML_NAME     "AX650X.xml"
#endif
#define XML_PREFIX_STR     "AX650X"
#define XML_POSTFIX_STR    ".xml"

#define MAX_PART_NAME_LEN   32
#define MAX_FILE_NAME_LEN   48

#define UBOOT_MAX_SIZE         (1536 * 1024)

struct update_part_info {
	char part_name[MAX_PART_NAME_LEN];    /* part name */
	u64 part_size;                        /* part size in Byte*/
	char file_name[MAX_FILE_NAME_LEN];    /* part bin name */
	u64 image_size;                       /* image size in Byte*/
	struct update_part_info *next;
};

extern int get_part_info_rawdata(struct update_part_info **part_list, char * src, int len);
extern int update_parts_info(struct update_part_info *pheader);
#ifdef VERSION_2_PARSE_XML
int update_parse_part_info(struct update_part_info **bin_info, struct blk_desc *blk_desc);
#endif

#endif
