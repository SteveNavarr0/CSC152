// p152.c by Steve Navarro. Submitted for CSC 152 February 22, 2026.

#include <stdint.h>
#include <string.h>

/* n must be 1-31 as a shift of 32 or more has undefined behavior */
uint32_t rotl(uint32_t x, int n) {
    return (x << n) | (x >> (32-n));
}

/* blk is a ptr to a 48 byte block, treated as array of 12 uint32_t.
 * xidx, yidx, zidx are the indices of the elements to mix
 */
void mix(uint32_t *blk, int xidx, int yidx, int zidx) {

	//copying original values of the id to be modified 
	uint32_t x = blk[xidx];
	uint32_t y = blk[yidx];
	uint32_t z = blk[zidx];
	  
	  //rotating the values to the left
	  x = rotl(x,24);
   	  y = rotl(y, 9);

	//the mixing process
	uint32_t zPrime = x ^ (z << 1) ^ ((y & z) << 2);
	uint32_t yPrime = y ^ x ^ ((x | z) << 1);
	uint32_t xPrime = z ^ y ^ ((x & y) << 3);

	  blk[xidx] = xPrime;
	  blk[yidx] = yPrime;
	  blk[zidx] = zPrime;

}

/* Perform p152 permutation on 48-byte input buffer "in" and write 48-byte
 * result to buffer "out". In and out may be the same or different buffers.
 * The two memcpys are not optimal but simplify the code.
 */
void p152(void *in, void *out) {
    uint32_t buf[12];
    memcpy(buf, in, 48);
    //implementing the diffusion
    for(int i = 0; i < 24; i++){
	
	//mixing the 4 columns
	mix(buf,0,4,8);
	mix(buf,1,5,9);
	mix(buf,2,6,10);
	mix(buf,3,7,11);
		
    	if((i % 4) == 0){
	   //swapping
	   uint32_t temp = buf[0];	
	   buf[0] = buf[1];
	   buf[1] = temp;

	   temp = buf[2];
	   buf[2] = buf[3];
	   buf[3] = temp;

	   buf[0] ^= 0xB7E15100;
	   buf[0] ^= (uint32_t)i;
	}

	if((i % 4) == 2){
	   uint32_t temp = buf[0];
	   buf[0] = buf[2];
	   buf[2] = temp;
	   temp = buf[1];
	   buf[1] = buf[3];
	   buf[3] = temp;
	}
    } // for loop
    memcpy(out, buf, 48);
}

/* key should be 32-byte random buffer, nonce should be 12-byte buffer that
 * is different for each call to p152stream using the same key. nbytes are
 * written to buffer specified by out. nbytes must be no more than 2^32 * 48.
 */
void p152stream(void *key, void *nonce, long nbytes, void *out) {
    unsigned char block[48], tmp[48];
    unsigned char *pout = out;
    uint32_t *ctr = (uint32_t *)(block + 44);  /* To increment last 4 bytes   */
    memcpy(block, key, 32);       /* Set block to key+nonce+zeroed counter    */
    memcpy(block+32, nonce, 12);
    memset(block+44, 0, 4);
    while (nbytes > 0) {
        int towrite = 48;
        p152(block, tmp);         /* Put never before used block through p152 */
        if (nbytes < 48)          /* Make towrite smaller if needed           */
            towrite = nbytes;
        for (int i = 0; i < towrite; i++) /* Xor p152 output with key to      */
            pout[i] = tmp[i] ^ block[i];  /* protect against inverse attack   */
        nbytes = nbytes - towrite;
        pout = pout + towrite;
        *ctr = *ctr + 1;
    }
}

#if 0  /* Set to 0 before submitting */
#include <stdio.h>

/* pbuf is used to print sequences of bytes from in memory         */
void pbuf(void *p, int len, char *s)
{
    if (s)
        printf("%s", s);
    for (int i=0; i<len; i++)
        printf("%02X", ((unsigned char *)p)[i]);
    printf("\n");
}

int main() {
    unsigned char buf[48];
    unsigned char out[48];
    for (int i=0; i<48; i++)
        buf[i] = (unsigned char)(i + 128);

    mix((uint32_t *)buf, 0, 1, 2);
    pbuf(buf, 12, "Mix : ");
    p152(buf, out);
    pbuf(out, 24, "p152: ");
    pbuf(out+24, 24, "p152: ");
    p152(buf, buf);
    if (memcmp(buf, out, 48))
        printf("p152 in-place error\n");

    return 0;
}

#endif
