/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet cin fdm=syntax : */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <jansson.h>

json_t *
item_load(FILE *fp)
{
	unsigned i;
	int c, inarr, instr, isesc;
	char *buf, *p;
	json_error_t error;
	json_t *res;

	inarr = instr = isesc = 0;
	i = 1;
	buf = malloc(sizeof(char));
	assert(buf != NULL);
	buf[0] = '{';
	do {
		c = fgetc(fp);
		assert(c != EOF);

		buf = realloc(buf, (i + 1) * sizeof(char));
		assert(buf != NULL);
		buf[i++] = c;

		if (isesc) {
			isesc = 0;
			if (c == '"')
				continue;
		}

		switch (c) {
		case '\\':
			if (instr)
				isesc = 1;
			break;
		case '"':
			instr = instr ? 0 : 1;
			break;
		case '[':
			if (!instr)
				++inarr;
			break;
		case ']':
			if (!instr)
				--inarr;
			break;
		default:
			break;
		}
	} while (instr || inarr || c != '}');

	buf = realloc(buf, (i + 1) * sizeof(char));
	assert(buf != NULL);
	buf[i++] = '\0';

	res = json_loads(buf, &error);
	if (!res) {
		fprintf(stderr, "Error loading `%s' on line:%d : %s", buf, error.line, error.text);
		free(buf);
		return NULL;
	}
	free(buf);
	return res;
}

void
item_print(json_t *item)
{
	unsigned i;
	json_t *thread, *timestamp, *matched, *total, *authors, *subject, *tags, *tag;

	thread = json_object_get(item, "thread");
	assert(json_is_string(thread));

	timestamp = json_object_get(item, "timestamp");
	assert(json_is_integer(timestamp));

	matched = json_object_get(item, "matched");
	assert(json_is_integer(matched));

	total = json_object_get(item, "total");
	assert(json_is_integer(total));

	authors = json_object_get(item, "authors");
	assert(json_is_string(authors));

	subject = json_object_get(item, "subject");
	assert(json_is_string(subject));

	printf("thread:%s  date:%d [%d/%d] %s; %s ( ",
			json_string_value(thread),
			json_integer_value(timestamp),
			json_integer_value(matched),
			json_integer_value(total),
			json_string_value(authors),
			json_string_value(subject));

	json_decref(thread);
	json_decref(timestamp);
	json_decref(matched);
	json_decref(total);
	json_decref(authors);
	json_decref(subject);

	tags = json_object_get(item, "tags");
	assert(json_is_array(tags));

	for (i = 0; i < json_array_size(tags); i++) {
		tag = json_array_get(tags, i);
		assert(json_is_string(tag));

		printf("%s ", json_string_value(tag));

		json_decref(tag);
	}
	printf(")\n");
	json_decref(tags);
}

int
main(int argc, char **argv)
{
	unsigned c;
	json_t *obj;

	/* Read '[' */
	while ((c = fgetc(stdin)) != '[') {
		if (c == EOF) {
			fprintf(stderr, "Hit EOF, expected [\n");
			return 1;
		}
	}

	for (;;) {
		while ((c = fgetc(stdin)) != '{') {
			if (c == EOF) {
				fprintf(stderr, "Hit EOF, expected {\n");
				return 1;
			}
			else if (c == ']')
				return 0;
		}

		obj = item_load(stdin);
		if (!obj)
			return 1;

		item_print(obj);
		json_decref(obj);
	}

	return 0;
}
