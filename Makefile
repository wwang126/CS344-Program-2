CC=gcc
CFLAGS=-g -Wall

all:
	${CC} keygen.c -o keygen ${CFLAGS}
	${CC} otp_enc.c -o otp_enc ${CFLAGS}
	${CC} otp_enc_d.c -o otp_enc_d ${CFLAGS}
	${CC} otp_dec.c -o otp_dec ${CFLAGS}
	${CC} otp_dec_d.c -o otp_dec_d ${CFLAGS}

keygen:
	${CC} keygen.c -o keygen ${CFLAGS}

enc:
	${CC} otp_enc.c -o otp_enc ${CFLAGS}
	${CC} otp_enc_d.c -o otp_enc_d ${CFLAGS}

dec:
	${CC} otp_dec.c -o otp_dec ${CFLAGS}
	${CC} otp_dec_d.c -o otp_dec_d ${CFLAGS}

clean:
	rm keygen
	rm otp_enc
	rm otp_enc_d
	rm otp_dec_d
	rm otp_dec 
