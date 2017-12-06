#include <cstdio> 
#include <windows.h>
#include <string>



//init hash 1
//计算出hash1 table 
//似乎和start.mjo有关,但是解析不能= = 
/*
004566A0  /$ 55             PUSH EBP
004566A1  |. 56             PUSH ESI
004566A2  |. 8B7424 0C      MOV ESI,DWORD PTR SS:[ESP+C]
004566A6  |. 83C9 FF        OR ECX,FFFFFFFF
004566A9  |. 33C0           XOR EAX,EAX
004566AB  |. 57             PUSH EDI
004566AC  |. 8BFE           MOV EDI,ESI
004566AE  |. F2:AE          REPNE SCAS BYTE PTR ES:[EDI]
004566B0  |. F7D1           NOT ECX
004566B2  |. 49             DEC ECX
004566B3  |. 8BE9           MOV EBP,ECX
004566B5  |. 8B0D F8385B00  MOV ECX,DWORD PTR DS:[5B38F8]
004566BB  |. 83C8 FF        OR EAX,FFFFFFFF
004566BE  |. 85C9           TEST ECX,ECX
004566C0  |. 75 40          JNZ SHORT Strawber.00456702
004566C2  |. 53             PUSH EBX
004566C3  |. 33FF           XOR EDI,EDI
004566C5  |. BE F82C5B00    MOV ESI,Strawber.005B2CF8
004566CA  |> 8BD7           /MOV EDX,EDI
004566CC  |. BB 08000000    |MOV EBX,8
004566D1  |> F6C2 01        |/TEST DL,1
004566D4  |. 74 0A          ||JE SHORT Strawber.004566E0
004566D6  |. D1EA           ||SHR EDX,1
004566D8  |. 81F2 2083B8ED  ||XOR EDX,EDB88320       ///const decoder seed
004566DE  |. EB 02          ||JMP SHORT Strawber.004566E2
004566E0  |> D1EA           ||SHR EDX,1
004566E2  |> 4B             ||DEC EBX
004566E3  |.^75 EC          |\JNZ SHORT Strawber.004566D1
004566E5  |. 8916           |MOV DWORD PTR DS:[ESI],EDX
004566E7  |. 83C6 04        |ADD ESI,4
004566EA  |. 47             |INC EDI
004566EB  |. 81FE F8305B00  |CMP ESI,Strawber.005B30F8
004566F1  |.^7C D7          \JL SHORT Strawber.004566CA
*/

//计算完成后的Xor Table
/*
00456937  |. 8BF2           MOV ESI,EDX
00456939  |. 8BCE           MOV ECX,ESI
0045693B  |. 81E1 FF000000  AND ECX,0FF
00456941  |. 33D2           XOR EDX,EDX
00456943  |. B8 F8285B00    MOV EAX,Strawber.005B28F8
00456948  |> 8D3C0A         /LEA EDI,DWORD PTR DS:[EDX+ECX]
0045694B  |. 81E7 FF000080  |AND EDI,800000FF
00456951  |. 79 08          |JNS SHORT Strawber.0045695B
00456953  |. 4F             |DEC EDI
00456954  |. 81CF 00FFFFFF  |OR EDI,FFFFFF00
0045695A  |. 47             |INC EDI
0045695B  |> 8B3CBD F82C5B0>|MOV EDI,DWORD PTR DS:[EDI*4+5B2CF8]
00456962  |. 33FE           |XOR EDI,ESI
00456964  |. 8938           |MOV DWORD PTR DS:[EAX],EDI
00456966  |. 83C0 04        |ADD EAX,4
00456969  |. 42             |INC EDX
0045696A  |. 3D F82C5B00    |CMP EAX,Strawber.005B2CF8
0045696F  |.^7C D7          \JL SHORT Strawber.00456948
00456971  |. 5F             POP EDI
00456972  |. 5E             POP ESI
00456973  |. 5D             POP EBP
00456974  \. C3             RETN
*/ 

//MOV EAX,あの晴れ.005C1FF8 
//这个就是Decoding Table 长度为1024*sizeof(BYTE) 
using namespace std;

typedef unsigned char u8;
typedef char s8;
typedef DWORD u32;
typedef WORD  u16;

#pragma pack (1)	
typedef struct
{				
	s8 magic[8];		/* "????TC00" and "????TC01" and "????TS00" and "????TS01" */
	u32 width;
	u32 height;
	u32 data_length;
} rct_header_t;

typedef struct {		
	s8 magic[16];		/* "MajiroObjX1.000" */
	u32 unknown0;
	u32 unknown1;
	u32 entries;		/* 8 bytes per entry */
	u8 *entries_buffer;
	u32 data_length;
	u8 *data_buffer;	
} mjo_header_t;

typedef struct {		/* ??????0x300????????? */			
	s8 magic[16];		/* "MajiroObjV1.000" */
	u32 unknown0;
	u32 unknown1;
	u32 entries;		/* 8 bytes per entry */
	u32 *entries_buffer;
	DWORD data_length;
	u8 *data_buffer;	
} mjoV_header_t;
#pragma pack ()	


static char table[1024] = 
{
0x10, 0x6E, 0xF8, 0xA8, 0x86, 0x5E, 0xFF, 0xDF,
0x25, 0xCB, 0x9B, 0x41, 0xB3, 0xFB, 0x9C, 0x36, 0x09, 0xAA, 0x95, 0xAF, 0x9F, 0x9A, 0x92, 0xD8,
0xA2, 0xB7, 0xF4, 0x6E, 0x34, 0x87, 0xF3, 0x19, 0x8E, 0xD6, 0xFA, 0x80, 0x18, 0xE6, 0xFD, 0xF7,
0xBB, 0x73, 0x99, 0x69, 0x2D, 0x43, 0x9E, 0x1E, 0x97, 0x12, 0x97, 0x87, 0x01, 0x22, 0x90, 0xF0,
0x90, 0x3F, 0x2F, 0x60, 0x06, 0x0F, 0x28, 0x17, 0xBC, 0x5E, 0x21, 0x8E, 0x2A, 0x6E, 0x26, 0xF9,
0x89, 0xFB, 0x42, 0x67, 0x1F, 0xCB, 0x45, 0x10, 0xA5, 0x9A, 0x4C, 0x89, 0x33, 0xAA, 0x4B, 0xFE,
0xC6, 0xA7, 0x43, 0x73, 0x50, 0x97, 0x44, 0x04, 0xEA, 0xC6, 0x4D, 0x9D, 0x7C, 0xF6, 0x4A, 0xEA,
0xDF, 0x63, 0x2E, 0x74, 0x49, 0x53, 0x29, 0x03, 0xF3, 0x02, 0x20, 0x9A, 0x65, 0x32, 0x27, 0xED,
0xF4, 0x2F, 0x98, 0x7D, 0x62, 0x1F, 0x9F, 0x0A, 0xD8, 0x4E, 0x96, 0x93, 0x4E, 0x7E, 0x91, 0xE4,
0xED, 0xEB, 0xF5, 0x7A, 0x7B, 0xDB, 0xF2, 0x0D, 0xC1, 0x8A, 0xFB, 0x94, 0x57, 0xBA, 0xFC, 0xE3,
0xDA, 0x55, 0xFE, 0xCE, 0x4C, 0x65, 0xF9, 0xB9, 0xF6, 0x34, 0xF0, 0x20, 0x60, 0x04, 0xF7, 0x57,
0xC3, 0x91, 0x93, 0xC9, 0x55, 0xA1, 0x94, 0xBE, 0xEF, 0xF0, 0x9D, 0x27, 0x79, 0xC0, 0x9A, 0x50,
0xE8, 0xDD, 0x25, 0xC0, 0x7E, 0xED, 0x22, 0xB7, 0xC4, 0xBC, 0x2B, 0x2E, 0x52, 0x8C, 0x2C, 0x59,
0xF1, 0x19, 0x48, 0xC7, 0x67, 0x29, 0x4F, 0xB0, 0xDD, 0x78, 0x46, 0x29, 0x4B, 0x48, 0x41, 0x5E,
0xBE, 0x45, 0x49, 0xD3, 0x28, 0x75, 0x4E, 0xA4, 0x92, 0x24, 0x47, 0x3D, 0x04, 0x14, 0x40, 0x4A,
0xA7, 0x81, 0x24, 0xD4, 0x31, 0xB1, 0x23, 0xA3, 0x8B, 0xE0, 0x2A, 0x3A, 0x1D, 0xD0, 0x2D, 0x4D,
0x8C, 0xCD, 0x92, 0xDD, 0x1A, 0xFD, 0x95, 0xAA, 0xA0, 0xAC, 0x9C, 0x33, 0x36, 0x9C, 0x9B, 0x44,
0x95, 0x09, 0xFF, 0xDA, 0x03, 0x39, 0xF8, 0xAD, 0xB9, 0x68, 0xF1, 0x34, 0x2F, 0x58, 0xF6, 0x43,
0x12, 0x75, 0x90, 0xF5, 0x84, 0x45, 0x97, 0x82, 0x3E, 0x14, 0x9E, 0x1B, 0xA8, 0x24, 0x99, 0x6C,
0x0B, 0xB1, 0xFD, 0xF2, 0x9D, 0x81, 0xFA, 0x85, 0x27, 0xD0, 0xF3, 0x1C, 0xB1, 0xE0, 0xF4, 0x6B,
0x20, 0xFD, 0x4B, 0xFB, 0xB6, 0xCD, 0x4C, 0x8C, 0x0C, 0x9C, 0x45, 0x15, 0x9A, 0xAC, 0x42, 0x62,
0x39, 0x39, 0x26, 0xFC, 0xAF, 0x09, 0x21, 0x8B, 0x15, 0x58, 0x28, 0x12, 0x83, 0x68, 0x2F, 0x65,
0x76, 0x65, 0x27, 0xE8, 0xE0, 0x55, 0x20, 0x9F, 0x5A, 0x04, 0x29, 0x06, 0xCC, 0x34, 0x2E, 0x71,
0x6F, 0xA1, 0x4A, 0xEF, 0xF9, 0x91, 0x4D, 0x98, 0x43, 0xC0, 0x44, 0x01, 0xD5, 0xF0, 0x43, 0x76,
0x44, 0xED, 0xFC, 0xE6, 0xD2, 0xDD, 0xFB, 0x91, 0x68, 0x8C, 0xF2, 0x08, 0xFE, 0xBC, 0xF5, 0x7F,
0x5D, 0x29, 0x91, 0xE1, 0xCB, 0x19, 0x96, 0x96, 0x71, 0x48, 0x9F, 0x0F, 0xE7, 0x78, 0x98, 0x78,
0x4A, 0x14, 0x22, 0xB8, 0xDC, 0x24, 0x25, 0xCF, 0x66, 0x75, 0x2C, 0x56, 0xF0, 0x45, 0x2B, 0x21,
0x53, 0xD0, 0x4F, 0xBF, 0xC5, 0xE0, 0x48, 0xC8, 0x7F, 0xB1, 0x41, 0x51, 0xE9, 0x81, 0x46, 0x26,
0x78, 0x9C, 0xF9, 0xB6, 0xEE, 0xAC, 0xFE, 0xC1, 0x54, 0xFD, 0xF7, 0x58, 0xC2, 0xCD, 0xF0, 0x2F,
0x61, 0x58, 0x94, 0xB1, 0xF7, 0x68, 0x93, 0xC6, 0x4D, 0x39, 0x9A, 0x5F, 0xDB, 0x09, 0x9D, 0x28,
0x2E, 0x04, 0x95, 0xA5, 0xB8, 0x34, 0x92, 0xD2, 0x02, 0x65, 0x9B, 0x4B, 0x94, 0x55, 0x9C, 0x3C,
0x37, 0xC0, 0xF8, 0xA2, 0xA1, 0xF0, 0xFF, 0xD5, 0x1B, 0xA1, 0xF6, 0x4C, 0x8D, 0x91, 0xF1, 0x3B,
0x1C, 0x8C, 0x4E, 0xAB, 0x8A, 0xBC, 0x49, 0xDC, 0x30, 0xED, 0x40, 0x45, 0xA6, 0xDD, 0x47, 0x32,
0x05, 0x48, 0x23, 0xAC, 0x93, 0x78, 0x24, 0xDB, 0x29, 0x29, 0x2D, 0x42, 0xBF, 0x19, 0x2A, 0x35,
0x82, 0x34, 0x4C, 0x83, 0x14, 0x04, 0x4B, 0xF4, 0xAE, 0x55, 0x42, 0x6D, 0x38, 0x65, 0x45, 0x1A,
0x9B, 0xF0, 0x21, 0x84, 0x0D, 0xC0, 0x26, 0xF3, 0xB7, 0x91, 0x2F, 0x6A, 0x21, 0xA1, 0x28, 0x1D,
0xB0, 0xBC, 0x97, 0x8D, 0x26, 0x8C, 0x90, 0xFA, 0x9C, 0xDD, 0x99, 0x63, 0x0A, 0xED, 0x9E, 0x14,
0xA9, 0x78, 0xFA, 0x8A, 0x3F, 0x48, 0xFD, 0xFD, 0x85, 0x19, 0xF4, 0x64, 0x13, 0x29, 0xF3, 0x13,
0xE6, 0x24, 0xFB, 0x9E, 0x70, 0x14, 0xFC, 0xE9, 0xCA, 0x45, 0xF5, 0x70, 0x5C, 0x75, 0xF2, 0x07,
0xFF, 0xE0, 0x96, 0x99, 0x69, 0xD0, 0x91, 0xEE, 0xD3, 0x81, 0x98, 0x77, 0x45, 0xB1, 0x9F, 0x00,
0xD4, 0xAC, 0x20, 0x90, 0x42, 0x9C, 0x27, 0xE7, 0xF8, 0xCD, 0x2E, 0x7E, 0x6E, 0xFD, 0x29, 0x09,
0xCD, 0x68, 0x4D, 0x97, 0x5B, 0x58, 0x4A, 0xE0, 0xE1, 0x09, 0x43, 0x79, 0x77, 0x39, 0x44, 0x0E,
0xFA, 0xD6, 0x46, 0x23, 0x6C, 0xE6, 0x41, 0x54, 0xD6, 0xB7, 0x48, 0xCD, 0x40, 0x87, 0x4F, 0xBA,
0xE3, 0x12, 0x2B, 0x24, 0x75, 0x22, 0x2C, 0x53, 0xCF, 0x73, 0x25, 0xCA, 0x59, 0x43, 0x22, 0xBD,
0xC8, 0x5E, 0x9D, 0x2D, 0x5E, 0x6E, 0x9A, 0x5A, 0xE4, 0x3F, 0x93, 0xC3, 0x72, 0x0F, 0x94, 0xB4,
0xD1, 0x9A, 0xF0, 0x2A, 0x47, 0xAA, 0xF7, 0x5D, 0xFD, 0xFB, 0xFE, 0xC4, 0x6B, 0xCB, 0xF9, 0xB3,
0x9E, 0xC6, 0xF1, 0x3E, 0x08, 0xF6, 0xF6, 0x49, 0xB2, 0xA7, 0xFF, 0xD0, 0x24, 0x97, 0xF8, 0xA7,
0x87, 0x02, 0x9C, 0x39, 0x11, 0x32, 0x9B, 0x4E, 0xAB, 0x63, 0x92, 0xD7, 0x3D, 0x53, 0x95, 0xA0,
0xAC, 0x4E, 0x2A, 0x30, 0x3A, 0x7E, 0x2D, 0x47, 0x80, 0x2F, 0x24, 0xDE, 0x16, 0x1F, 0x23, 0xA9,
0xB5, 0x8A, 0x47, 0x37, 0x23, 0xBA, 0x40, 0x40, 0x99, 0xEB, 0x49, 0xD9, 0x0F, 0xDB, 0x4E, 0xAE,
0x32, 0xF6, 0x28, 0x18, 0xA4, 0xC6, 0x2F, 0x6F, 0x1E, 0x97, 0x26, 0xF6, 0x88, 0xA7, 0x21, 0x81,
0x2B, 0x32, 0x45, 0x1F, 0xBD, 0x02, 0x42, 0x68, 0x07, 0x53, 0x4B, 0xF1, 0x91, 0x63, 0x4C, 0x86,
0x00, 0x7E, 0xF3, 0x16, 0x96, 0x4E, 0xF4, 0x61, 0x2C, 0x1F, 0xFD, 0xF8, 0xBA, 0x2F, 0xFA, 0x8F,
0x19, 0xBA, 0x9E, 0x11, 0x8F, 0x8A, 0x99, 0x66, 0x35, 0xDB, 0x90, 0xFF, 0xA3, 0xEB, 0x97, 0x88,
0x56, 0xE6, 0x9F, 0x05, 0xC0, 0xD6, 0x98, 0x72, 0x7A, 0x87, 0x91, 0xEB, 0xEC, 0xB7, 0x96, 0x9C,
0x4F, 0x22, 0xF2, 0x02, 0xD9, 0x12, 0xF5, 0x75, 0x63, 0x43, 0xFC, 0xEC, 0xF5, 0x73, 0xFB, 0x9B,
0x64, 0x6E, 0x44, 0x0B, 0xF2, 0x5E, 0x43, 0x7C, 0x48, 0x0F, 0x4A, 0xE5, 0xDE, 0x3F, 0x4D, 0x92,
0x7D, 0xAA, 0x29, 0x0C, 0xEB, 0x9A, 0x2E, 0x7B, 0x51, 0xCB, 0x27, 0xE2, 0xC7, 0xFB, 0x20, 0x95,
0x6A, 0x97, 0x9A, 0x55, 0xFC, 0xA7, 0x9D, 0x22, 0x46, 0xF6, 0x94, 0xBB, 0xD0, 0xC6, 0x93, 0xCC,
0x73, 0x53, 0xF7, 0x52, 0xE5, 0x63, 0xF0, 0x25, 0x5F, 0x32, 0xF9, 0xBC, 0xC9, 0x02, 0xFE, 0xCB,
0x58, 0x1F, 0x41, 0x5B, 0xCE, 0x2F, 0x46, 0x2C, 0x74, 0x7E, 0x4F, 0xB5, 0xE2, 0x4E, 0x48, 0xC2,
0x41, 0xDB, 0x2C, 0x5C, 0xD7, 0xEB, 0x2B, 0x2B, 0x6D, 0xBA, 0x22, 0xB2, 0xFB, 0x8A, 0x25, 0xC5,
0x0E, 0x87, 0x2D, 0x48, 0x98, 0xB7, 0x2A, 0x3F, 0x22, 0xE6, 0x23, 0xA6, 0xB4, 0xD6, 0x24, 0xD1,
0x17, 0x43, 0x40, 0x4F, 0x81, 0x73, 0x47, 0x38, 0x3B, 0x22, 0x4E, 0xA1, 0xAD, 0x12, 0x49, 0xD6,
0x3C, 0x0F, 0xF6, 0x46, 0xAA, 0x3F, 0xF1, 0x31
};


static u32 hash_table[256];
static u32 hash_table2[256];

static void init_hash(void)
{
	unsigned int i, k;

	for (k = 0; k < 256; k++) {	
		DWORD flag = k;
		for (i = 0; i < 8; i++) {
			if (flag & 1)
				flag = (flag >> 1) ^ 0xEDB88320;
			else 
				flag >>= 1;
		}
		hash_table[k] = flag;
	}
}

static DWORD do_hash(DWORD seed, BYTE *name, DWORD name_len)
{
	for (unsigned int i = 0; i < name_len; i++)
		seed = hash_table[name[i] ^ (seed & 0xff)] ^ (seed >> 8);
	return ~seed;
}

static void init_hash2(BYTE *name, DWORD name_len)
{
	u32 hash;

	hash = do_hash(-1, name, name_len);
	for (unsigned int i = 0; i < 256; i++)	
		hash_table2[i] = hash ^ hash_table[(i + (BYTE)hash) % 256];	
}


static void find_ts_key(BYTE *in, DWORD size, 
						BYTE *ret_key, DWORD *ret_key_len)
{
	BYTE *end = in + size;

	while (in < end) {
		if (*(u16 *)in != 0x0801) {
			++in;
			continue;
		}

		in += 2;
		short key_len = *(u16 *)in;
		if (key_len > 4 && key_len <= 64 && (in + key_len < end)) {
			*ret_key_len = key_len - 1;
			strcpy((char *)ret_key, (char *)in + 2);
			break;
		}
	}
}

static int dump_mjo(BYTE *buf, DWORD length, BYTE *ret_buf, DWORD *ret_len)
{
	mjo_header_t mjo;
	BYTE *tmp = buf;
	BYTE *htable = (BYTE *)hash_table;

	mjo.unknown0 = *(DWORD *)(buf + 16);
	buf += 20;

	mjo.unknown1 = *(DWORD *)buf;
	buf += 4;
	mjo.entries = *(DWORD *)buf;
	buf += 4;
	mjo.entries_buffer = buf;
	buf += mjo.entries * 8;
	mjo.data_length = *(DWORD *)buf;
	buf += 4;
	mjo.data_buffer = buf;

	if (buf + mjo.data_length != tmp + length)
		return -1;

	ret_buf = (BYTE *)malloc(mjo.data_length);
	if (!*ret_buf)
		return -1;

//	printf("entries %d, unknown0 %d, unknown1 %d, data length %d bytes\n",
//		mjo.entries, mjo.unknown0, mjo.unknown1, mjo.data_length);

	for (unsigned int i = 0; i < mjo.data_length; i++)
		ret_buf[i] = mjo.data_buffer[i] ^ htable[i & (sizeof(hash_table) - 1)];

	*ret_len = mjo.data_length;

	return 0;
}

/***********************************************************************/

static DWORD rct_decompress(BYTE *uncompr, DWORD uncomprLen, 
		BYTE *compr, DWORD comprLen, DWORD width)
{
	DWORD act_uncomprLen = 0;
	DWORD curByte = 0;
	DWORD pos[32];
	
	pos[0] = -3;
	pos[1] = -6;
	pos[2] = -9;
	pos[3] = -12;	
	pos[4] = -15;
	pos[5] = -18;
	pos[6] = (3 - width) * 3;	
	pos[7] = (2 - width) * 3;		
	pos[8] = (1 - width) * 3;
	pos[9] = (0 - width) * 3;
	pos[10] = (-1 - width) * 3;		
	pos[11] = (-2 - width) * 3;
	pos[12] = (-3 - width) * 3;		
	pos[13] = 9 - ((width * 3) << 1);
	pos[14] = 6 - ((width * 3) << 1);
	pos[15] = 3 - ((width * 3) << 1);	
	pos[16] = 0 - ((width * 3) << 1);	
	pos[17] = -3 - ((width * 3) << 1);		
	pos[18] = -6 - ((width * 3) << 1);
	pos[19] = -9 - ((width * 3) << 1);
	pos[20] = 9 - width * 9;	
	pos[21] = 6 - width * 9;
	pos[22] = 3 - width * 9;	
	pos[23] = 0 - width * 9;	
	pos[24] = -3 - width * 9;
	pos[25] = -6 - width * 9;	
	pos[26] = -9 - width * 9;
	pos[27] = 6 - ((width * 3) << 2);
	pos[28] = 3 - ((width * 3) << 2);
	pos[29] = 0 - ((width * 3) << 2);
	pos[30] = -3 - ((width * 3) << 2);	
	pos[31] = -6 - ((width * 3) << 2);	
						
	uncompr[act_uncomprLen++] = compr[curByte++];
	uncompr[act_uncomprLen++] = compr[curByte++];	
	uncompr[act_uncomprLen++] = compr[curByte++];
		
	while (1) {
		BYTE flag;
		DWORD copy_bytes, copy_pos;
		
		if (curByte >= comprLen)
			break;	

		flag = compr[curByte++];
			
		if (!(flag & 0x80)) {
			if (flag != 0x7f)
				copy_bytes = flag * 3 + 3;
			else {
				if (curByte + 1 >= comprLen)
					break;
										
				copy_bytes = compr[curByte++];
				copy_bytes |= compr[curByte++] << 8;
				copy_bytes += 0x80;
				copy_bytes *= 3;
			}
			
			if (curByte + copy_bytes - 1 >= comprLen)
				break;	
			if (act_uncomprLen + copy_bytes - 1 >= uncomprLen)
				break;
								
			memcpy(&uncompr[act_uncomprLen], &compr[curByte], copy_bytes);
			act_uncomprLen += copy_bytes;
			curByte += copy_bytes;	
		} else {
			copy_bytes = flag & 3;
			copy_pos = (flag >> 2) & 0x1f;
			
			if (copy_bytes != 3) {
				copy_bytes = copy_bytes * 3 + 3;
			} else {
				if (curByte + 1 >= comprLen)
					break;	
					
				copy_bytes = compr[curByte++];
				copy_bytes |= compr[curByte++] << 8;
				copy_bytes += 4;
				copy_bytes *= 3;			
			}
			
			for (unsigned int i = 0; i < copy_bytes; i++) {
				if (act_uncomprLen >= uncomprLen)
				{
					//printf("overflow!\n");
					goto out;
				}	
				uncompr[act_uncomprLen] = uncompr[act_uncomprLen + pos[copy_pos]];	
				act_uncomprLen++;			
			}
		}	
	}
out:
/*
	if (curByte != comprLen)
	{
		MessageBoxA(NULL, "Decode Error!", "Error", MB_OK);
		//printf("compr miss-match %d VS %d\n", curByte, comprLen);
	//	system("pause");
	}
	*/
	return act_uncomprLen;		
}


int CALLBACK WriteBMP32A(const string&    filename, unsigned char* buff, unsigned long len,
						 unsigned long    width,
               			 unsigned long    height,
               			 unsigned short   depth)
{
	BITMAPFILEHEADER bmf;
  	BITMAPINFOHEADER bmi;

  	memset(&bmf, 0, sizeof(bmf));
  	memset(&bmi, 0, sizeof(bmi));

  	bmf.bfType     = 0x4D42;
  	bmf.bfSize     = sizeof(bmf) + sizeof(bmi) + len;
  	bmf.bfOffBits  = sizeof(bmf) + sizeof(bmi);

  	bmi.biSize     = sizeof(bmi);
  	bmi.biWidth    = width;
  	bmi.biHeight   = height;
  	bmi.biPlanes   = 1;
  	bmi.biBitCount = depth *8;

	/*
	char pDic[260];
	memset(pDic,0,sizeof(pDic));
	GetCurrentDirectoryA(260, pDic);
  	*/

  	FILE* fp = fopen(filename.c_str(), "wb");
  	fwrite(&bmf, sizeof(bmf), 1, fp);
  	fwrite(&bmi, sizeof(bmi), 1, fp);
  	fwrite(buff, len,         1, fp);
  	fclose(fp);
  	return 0;
}


static int dump_rct1(rct_header_t *rct, const string& file)
{
	BYTE *compr, *uncompr;
	DWORD uncomprLen, comprLen, actLen;
	WORD fn_len;

	uncomprLen = rct->width * rct->height * 3;
	uncompr = (BYTE *)malloc(uncomprLen);
	if (!uncomprLen)
		return -1;	
	
	compr = (BYTE *)(rct + 1);
	fn_len = *(WORD *)compr;
	compr += 2;
	compr += fn_len;
	comprLen = rct->data_length;
	
	actLen = rct_decompress(uncompr, uncomprLen, compr, comprLen, rct->width);
	if (actLen != uncomprLen) {
//		fprintf(stderr, "rct decompress error occured (%d VS %d)\n",
//			actLen, uncomprLen);
		MessageBoxA(NULL, "RCT01 Decode Error!", "Error", MB_OK);
		free(uncompr);	
		return -1;
	}

	WriteBMP32A(file, uncompr, uncomprLen, rct->width, 0 - rct->height, 24/8);
	free(uncompr);
		
	return 0;		
}

static int dump_rct(rct_header_t *rct, const string &file)
{
	BYTE *compr, *uncompr;
	DWORD uncomprLen, comprLen, actLen;

	uncomprLen = rct->width * rct->height * 3;
	uncompr = (BYTE *)malloc(uncomprLen);
	if (!uncomprLen)
		return -1;	
	
	compr = (BYTE *)(rct + 1);	
	comprLen = rct->data_length;
	actLen = rct_decompress(uncompr, uncomprLen, compr, comprLen, rct->width);
	if (actLen != uncomprLen) {
//		fprintf(stderr, "rct decompress error occured (%d VS %d)\n",
//			actLen, uncomprLen);
		MessageBoxA(NULL, "RCT00 Decode Error!", "Error", MB_OK);
		free(uncompr);	
		return -1;
	}

	WriteBMP32A(file, uncompr, uncomprLen, rct->width, 0 - rct->height, 24/8);
	
	free(uncompr);
		
	return 0;		
}


static int dump_ts00(rct_header_t *rct, const string &file)
{
	BYTE *compr, *uncompr;
	DWORD uncomprLen, comprLen, actLen;
	BYTE *htable = (BYTE *)hash_table2;

	uncomprLen = rct->width * rct->height * 3;
	uncompr = (BYTE *)malloc(uncomprLen);
	if (!uncomprLen)
		return -1;	
	
	compr = (BYTE *)(rct + 1);	
	comprLen = rct->data_length;
	for (unsigned int i = 0; i < comprLen; i++)
		compr[i] ^= table[i & (sizeof(table) - 1)];

	actLen = rct_decompress(uncompr, uncomprLen, compr, comprLen, rct->width);
	if (actLen != uncomprLen) {
		free(uncompr);	
		return -1;
	}

	WriteBMP32A(file, uncompr, uncomprLen, rct->width, 0 - rct->height, 24/8);
	free(uncompr);
		
	return 0;		
}

static int dump_ts01(rct_header_t *rct, const string &file)
{
	BYTE *compr, *uncompr;
	DWORD uncomprLen, comprLen, actLen;
	BYTE *htable = (BYTE *)hash_table2;
	WORD fn_len;

	uncomprLen = rct->width * rct->height * 3;
	uncompr = (BYTE *)malloc(uncomprLen);
	if (!uncomprLen)
		return -1;	
	
	compr = (BYTE *)(rct + 1);
	fn_len = *(WORD *)compr;
	compr += 2;
	compr += fn_len;
	comprLen = rct->data_length;
	for (unsigned int i = 0; i < comprLen; i++)
		compr[i] ^= table[i & (sizeof(table) - 1)];
	actLen = rct_decompress(uncompr, uncomprLen, compr, comprLen, rct->width);
	if (actLen != uncomprLen) {
		free(uncompr);	
		return -1;
	}


	WriteBMP32A(file, uncompr, uncomprLen, rct->width, 0 - rct->height, 24/8);

	free(uncompr);
		
	return 0;		
}




int main(int argc, char **argv)
{
	bool isHasKey = false; 
	if(argc != 2 && argc != 3)
	{
		MessageBoxA(NULL, "Usage: MajiroDecoder <infile> <start.mjo>\n<start.mjo> is an alternative option", "Majiro Tool", MB_OK);
		return -2;
	}
	
	if(argc == 2)
	{
		printf("Some resources will never be extracted!\n");
	}
	
	if(argc == 3)
	{
		isHasKey = true;
	}
	
	init_hash();
	
	
	if(isHasKey)
	{
	FILE *key = fopen(argv[2],"rb");
	if(key == NULL)
		return -1;
	fseek(key,0,SEEK_END);
	DWORD KeyFileSize=ftell(key);
	rewind(key);
	char *kFile=new char [KeyFileSize];
	fread(kFile,KeyFileSize,1,key);
	fclose(key);
	BYTE _ts_key[MAX_PATH];
	DWORD _ts_key_len = 0;
	
	BYTE *pDecodeBuf = new BYTE[KeyFileSize];
	DWORD ret_len = 0;
	
	dump_mjo((BYTE*)kFile, KeyFileSize, pDecodeBuf, &ret_len);
	
	find_ts_key(pDecodeBuf, ret_len > 128 ? 128 : ret_len, _ts_key, &_ts_key_len);
	if (!_ts_key_len)
	{
		MessageBoxA(NULL, "Failed to automatically calculate decoding data!", "warning", MB_OK);
	}
	else
		init_hash2(_ts_key, _ts_key_len);

	}
	
	FILE *fin = fopen(argv[1],"rb");
	printf("%s\n", argv[1]);
	if(fin == NULL)
		return -1;
	fseek(fin,0,SEEK_END);
	DWORD FileSize=ftell(fin);
	rewind(fin);
	char *pFile=new char [FileSize];
	fread(pFile,FileSize,1,fin);
	fclose(fin);
	
	//filename = argv[1];
	//filename += ".bmp";
	string filename = argv[1];
	filename += ".bmp";
	
	if (!memcmp(pFile, "\x98\x5a\x92\x9aTC00", 8))
	{
		if (dump_rct((rct_header_t *)pFile, filename))
		{
			delete[] pFile;
			return -1;		
		}
	}
	else if (!memcmp(pFile, "\x98\x5a\x92\x9aTC01", 8))
	{
		if (dump_rct1((rct_header_t *)pFile, filename))
		{
			delete[] pFile;
			return -1;		
		}
	}
	else if (!memcmp(pFile, "\x98\x5a\x92\x9aTS00", 8))
	{
		if (dump_ts00((rct_header_t *)pFile, filename))
		{
			delete[] pFile;
			return -1;		
		}
	}
	else if (!memcmp(pFile, "\x98\x5a\x92\x9aTS01", 8))
	{
		if (dump_ts01((rct_header_t *)pFile, filename))
		{
			delete[] pFile;
			return -1;		
		}
	}
	delete[] pFile;
	return 0; 
}
