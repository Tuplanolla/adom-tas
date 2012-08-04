/**
Rolls characters.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef ROLL_C
#define ROLL_C

#include <stddef.h>//NULL

#include "util.h"

int rollstage = 0;
bool rollasked[51];
char answers[51];
int qnum = 0;

char qathing(const int question, const int * const attreqs) {//TODO refactor without breaking
	if (rollasked[question]) return '?';
	if (answers[question] != '?') return answers[question];
	rollasked[question] = TRUE;
	int score[4] = {0, 0, 0, 0};
	for (size_t opt = 0; opt < 4; opt++) {
		int weight = 1;
		for (size_t atr = 0; atr < 9; atr++) {
			int zorg = attreqs[8 - atr];
			score[opt] += weight * exec_question_effects[question][opt][zorg];
			weight *= 2;
		}
	}
	int answer = 4;
	int max = -1 << 31;
	for (size_t opt = 0; opt < 4; opt++) {
		if (score[opt] > max) {
			max = score[opt];
			answer = (int )opt;
		}
	}
	const char letters[5] = {'a', 'b', 'c', 'd', '?'};
	answers[question] = letters[answer];
	return letters[answer];
}

int rollthing(void) {
	rollstage++;
	switch (rollstage) {
		case -127: {
			int * birthday = (int * )0x082b61f0;
			int * gender = (int * )0x082add18;
			int * race = (int * )0x082add10;
			int * prof = (int * )0x082add14;
			int * gift = (int * )0x082b6144;
			int * attributes = (int * )0x082b1728;
			int * items = (int * )0x082a5980;
			int * books = (int * )0x082a7e00;
			if (books[0x14] == 0
					|| books[0x1e] == 0
					|| items[0xa9] == 0
					|| attributes[0x01] < 20
					|| attributes[0x07] < 20) exit(0);
			char buf[32];
			snprintf(buf, sizeof buf, "cat/%u.tac", (unsigned int )timestamp);
			FILE * const f = fopen(buf, "wb");
			if (f != NULL) {
				const unsigned char header[4] = {'T', 'A', 'C', '\0'};
				fwrite(header, sizeof header, 0x01, f);
				fwrite("adom", 4, 0x01, f);
				fwrite((char [1024] ){[0 ... 1023] = '\0'}, 1, 1016, f);
				fwrite(birthday, sizeof (int), 0x01, f);
				fwrite(gender, sizeof (int), 0x01, f);
				fwrite(race, sizeof (int), 0x01, f);
				fwrite(prof, sizeof (int), 0x01, f);
				fwrite(answers, sizeof answers + 1, 0x01, f);
				fwrite(gift, sizeof (int), 0x01, f);
				fwrite(attributes, sizeof (int), 0x09, f);
				fwrite(items, sizeof (int), 0x2b9, f);
				fwrite(books, sizeof (int), 0x2f, f);
				fclose(f);
				exit(0);
			}
			exit(1);
		}
		case 1: return 'g';
		case 2: return ' ';
		case 3: return 's';
		case 4: return 'm';
		case 5: return 'g';
		case 6: return 'f';
		case 7: return ' ';
		case 8: {
			for (size_t question = 0; question < 51; question++) {
				answers[question] = '?';
			}
			return 'q';
		}
		default: {
			const int attreqs[9] = {1, 7, 2, 4, 3, 0, 8, 5, 6};//Le > Ma > Wi > To > Dx > St > Pe > Ch > Ap
			char result = qathing(exec_questions[qnum], attreqs);
			qnum++;
			if (result == '?') {
				rollstage = -128;
			}
			return result;
		}
	}
	return 0;
}

/*
	if (record.count == 0) {//move to roll
		rolling = TRUE;
		timestamp--;
		goto front;
		back: timestamp++;
		struct tm * tm;
		tm = gmtime(&timestamp);
		if (!(tm->tm_mon == 11 && tm->tm_mday == 31)) {
			tm->tm_sec = 0;
			tm->tm_min = 0;
			tm->tm_hour = 0;
			tm->tm_mday = 31;
			tm->tm_mon = 11;
			tm->tm_isdst = 0;
			timestamp = mktime(tm) - timezone;
		}
		front: iarc4((unsigned int )timestamp, 0);
		for (size_t question = 0; question < 51; question++) {
			rollasked[question] = FALSE;
		}
		if (fork() > 0) {
			int s;
			wait(&s);
			if (s == 0) goto back;
		}
		else skipwr = TRUE;
		goto r0;
	}
*/

#endif
