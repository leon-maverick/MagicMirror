/**
 * example C code using libcurl and json-c
 * to post and return a payload using
 * http://jsonplaceholder.typicode.com
 *
 * Requirements:
 *
 * json-c - https://github.com/json-c/json-c
 * libcurl - http://curl.haxx.se/libcurl/c
 *
 * Build:
 *
 * cc curltest.c -lcurl -ljson-c -o curltest
 *
 * Run:
 *
 * ./curltest
 *
 */

/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <linux/rtc.h>

/* json-c (https://github.com/json-c/json-c) */
#include <json-c/json.h>

/* libcurl (http://curl.haxx.se/libcurl/c) */
#include <curl/curl.h>

/* holder for curl fetch */
struct curl_fetch_st {
    char *payload;
    size_t size;
};

/* callback for curl fetch */
size_t curl_callback (void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;                             /* calculate buffer size */
    struct curl_fetch_st *p = (struct curl_fetch_st *) userp;   /* cast pointer to fetch struct */

    /* expand buffer */
    p->payload = (char *) realloc(p->payload, p->size + realsize + 1);

    /* check buffer */
    if (p->payload == NULL) {
      /* this isn't good */
      fprintf(stderr, "ERROR: Failed to expand buffer in curl_callback");
      /* free buffer */
      free(p->payload);
      /* return */
      return -1;
    }

    /* copy contents to buffer */
    memcpy(&(p->payload[p->size]), contents, realsize);

    /* set new buffer size */
    p->size += realsize;

    /* ensure null termination */
    p->payload[p->size] = 0;

    /* return size */
    return realsize;
}

/* fetch and return url body via curl */
CURLcode curl_fetch_url(CURL *ch, const char *url, struct curl_fetch_st *fetch) {
    CURLcode rcode;                   /* curl result code */

    /* init payload */
    fetch->payload = (char *) calloc(1, sizeof(fetch->payload));

    /* check payload */
    if (fetch->payload == NULL) {
        /* log error */
        fprintf(stderr, "ERROR: Failed to allocate payload in curl_fetch_url");
        /* return error */
        return CURLE_FAILED_INIT;
    }

    /* init size */
    fetch->size = 0;

    /* set url to fetch */
    curl_easy_setopt(ch, CURLOPT_URL, url);

    /* set calback function */
    curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, curl_callback);

    /* pass fetch struct pointer */
    curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *) fetch);

    /* set default user agent */
    curl_easy_setopt(ch, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* set timeout */
    curl_easy_setopt(ch, CURLOPT_TIMEOUT, 5);

    /* enable location redirects */
    curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);

    /* set maximum allowed redirects */
    curl_easy_setopt(ch, CURLOPT_MAXREDIRS, 1);

    /* fetch the url */
    rcode = curl_easy_perform(ch);

    /* return */
    return rcode;
}

char* SendCurl(char *url){

    CURL *ch;                                               /* curl handle */
    CURLcode rcode;                                         /* curl result code */

    json_object *json;                                      /* json post body */
    enum json_tokener_error jerr = json_tokener_success;    /* json parse error */

    struct curl_fetch_st curl_fetch;                        /* curl fetch struct */
    struct curl_fetch_st *cf = &curl_fetch;                 /* pointer to fetch struct */
    struct curl_slist *headers = NULL;                      /* http headers to send with request */

    /* init curl handle */
    if ((ch = curl_easy_init()) == NULL) {
        /* log error */
        fprintf(stderr, "ERROR: Failed to create curl handle in fetch_session");
        /* return error */
        return "a" ;
    }

    /* set content type */
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");

    /* create json object for get */
    json = json_object_new_object();

    /* set curl options */
    curl_easy_setopt(ch, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(ch, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(ch, CURLOPT_POSTFIELDS, json_object_to_json_string(json));

    /* fetch page and capture return code */
    rcode = curl_fetch_url(ch, url, cf);

    /* cleanup curl handle */
    curl_easy_cleanup(ch);

    /* free headers */
    curl_slist_free_all(headers);

    /* free json object */
    json_object_put(json);

    /* check return code */
    if (rcode != CURLE_OK || cf->size < 1) {
        /* log error */
        fprintf(stderr, "ERROR: Failed to fetch url (%s) - curl said: %s \n",
            url, curl_easy_strerror(rcode));
        /* return error */
        return "b";
    }

    /* check payload */
    if (cf->payload != NULL) {
        /* print result */
//        printf("CURL Returned: \n%s\n", cf->payload); That was there for checking
        /* parse return */
        json = json_tokener_parse_verbose(cf->payload, &jerr);
        /* free payload */
        free(cf->payload);
    } else {
        /* error */
        fprintf(stderr, "ERROR: Failed to populate payload");
        /* free payload */
        free(cf->payload);
        /* return */
        return "c";
    }

    /* check error */
    if (jerr != json_tokener_success) {
        /* error */
        fprintf(stderr, "ERROR: Failed to parse json string\n");
        /* free json object */
        json_object_put(json);
        /* return */
        return "d";
    }

    /* debugging */
    printf("Parsed JSON:\n%s\n", json_object_to_json_string(json));

 FILE *fptr;
    fptr=fopen("myconfig.txt","w");
   if(fptr==NULL){
      printf("Error!");
      exit(1);
   }
   fprintf(fptr,"%s",json_object_to_json_string(json));
   fclose(fptr);
    /* exit */
    return (char*)json_object_to_json_string(json) ;
    
}


int ReadRTC(void)
{
	time_t mytime;
	mytime = time(NULL);
	printf("%s",ctime(&mytime));
		return 0;
}



int main(int argc, char *argv[]) {

  //char *url = "http://api.worldweatheronline.com/free/v2/weather.ashx?q=Tehran&format=json&num_of_days=1&key=f793dd5de838b6be5b4a401294b09";
  //  char *url = "http://api.worldweatheronline.com/free/v2/tz.ashx?q=Babol&key=f793dd5de838b6be5b4a401294b09&format=json";
    char *url = "https://mirror.nestous.com/mirror/api.php/dongle?session_id=f8cdd52d85a1b3591ac97d3ece8e80b1&secret=e5db79e639c323ece28618457e4f8a0e&version=1";
    char *ret_json = SendCurl(url);
    ReadRTC();
    return 0;
}
