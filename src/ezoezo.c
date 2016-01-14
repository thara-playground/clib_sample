#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "console-colors/console-colors.h"
#include "parson/parson.h"
#include "http-get/http-get.h"

int main(int argc, char* argv[]) {
    http_get_response_t* res = http_get("http://cpplover.blogspot.com/feeds/posts/default?alt=json");
    if (res->status != 200) {
        cc_fprintf(CC_FG_RED, stderr, "ERROR");
        goto leave;
    }

    char* json = calloc(res->size + 1, 1);
    if (!json) goto leave;
    strncpy(json, res->data, res->size);

    JSON_Value *root_value = json_parse_string(json);
    JSON_Object *root = json_value_get_object(root_value);
    JSON_Array *entries = json_object_dotget_array(root, "feed.entry");
    for (int i = 0; i < json_array_get_count(entries); i++) {
        JSON_Object *entry = json_array_get_object(entries, i);
        cc_fprintf(CC_FG_BLUE, stdout, "%s\n", json_object_dotget_string(entry, "title.$t"));
        JSON_Array *links = json_object_get_array(entry, "link");
        for (int l = 0; l < json_array_get_count(links); l++) {
            JSON_Object *link = json_array_get_object(links, l);
            if (!strcmp("alternate", json_object_get_string(link, "rel"))) {
                cc_fprintf(CC_FG_YELLOW, stdout, "%s\n", json_object_dotget_string(link, "href"));
                break;
            }
        }
    }
    json_value_free(root_value);

leave:
  if (res) http_get_free(res);
  if (json) free(json);
  return 0;
}
