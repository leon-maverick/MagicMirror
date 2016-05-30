/**Requirements:
 * json-c - https://github.com/json-c/json-c
 * libcurl - http://curl.haxx.se/libcurl/c
 * Build:
 * cc curltest.c -lcurl -ljson-c -o curltest
 * Run:
 * ./curltest
 */
// install iwlib-dev iwlib30 apt-cache search libiw

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/time.h>
#include <signal.h>
#include <iwlib.h>


#define INTERVAL 5000        /* number of milliseconds to go off */

/* json-c (https://github.com/json-c/json-c) */
#include <json-c/json.h>

/* libcurl (http://curl.haxx.se/libcurl/c) */
#include <curl/curl.h>

volatile int abortt = 0;
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
    curl_easy_setopt(ch, CURLOPT_TIMEOUT, 10);

    /* enable location redirects */
    curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);

    /* set maximum allowed redirects */
    curl_easy_setopt(ch, CURLOPT_MAXREDIRS, 1);

    /* fetch the url */
    rcode = curl_easy_perform(ch);

    /* return */
    return rcode;
}

char* readFile(char* filename){
    FILE *fp;
    long lSize;
    char *buffer;
    
    fp = fopen ( filename , "rb" );
    if( !fp ) perror(filename),exit(1);
    
    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );
    rewind( fp );
    
    /* allocate memory for entire content */
    buffer = calloc( 1, lSize+1 );
    if( !buffer ) fclose(fp),fputs("memory alloc fails",stderr),exit(1);
    
    /* copy the file into the buffer */
    if( 1!=fread( buffer , lSize, 1 , fp) ){
        fclose(fp);
        free(buffer);
        //fputs("entire read fails\n",stderr);
        printf("Cant read %s\n", filename);
        return(NULL);
    }
    //printf("%lu\n",strlen(buffer) );
    /* do your work here, buffer is a string contains the whole text */
    fclose(fp);
    return (buffer);
}

void getMACAddress(char* _iface) {
    unsigned char MAC[6];
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , _iface , IFNAMSIZ-1);
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    int i = 0 ;
    for( i=0;i<6;i++)
        MAC[i] = ifr.ifr_hwaddr.sa_data[i];
    ioctl(fd, SIOCGIFMTU, &ifr);
    close(fd);
    //printf("MTU: %d\n",ifr.ifr_mtu);
    printf("MAC:%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",MAC[0],MAC[1],MAC[2],MAC[3],MAC[4],MAC[5]);
    FILE* frp;
    frp=fopen("uuid.txt","w");
    if( !frp ) {
        perror("uuid.txt");
        return;
    }

    fprintf(frp,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",MAC[0],MAC[1],MAC[2],MAC[3],MAC[4],MAC[5]);
    fclose(frp);
}

int hotspot(){
    // make hotspot and get some data
    printf("hotspot!hotspot!hotspot!\n");
    // if connection was successful and recived data return 1 else retun 0 
    return 1;
}

int InternetAccess(){
    struct sockaddr_in servaddr;
    struct hostent *hp;
    int sock_id;
    char message[1024*1024];
    char msglen;
    char request[] = "GET /index.html HTTP/1.0\n"
    "From: slava!!!\nUser-Agent: wget_sortof by slava\n\n";
    
    //Get a socket  
    if((sock_id = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr,"Couldn't get a socket.\nx");// exit(EXIT_FAILURE);
        return 2;
    }
    else {
        fprintf(stderr,"Got a socket.\n");
    }
    
    //book uses bzero which my man pages say is deprecated
    //the man page said to use memset instead. :-)
    memset(&servaddr,0,sizeof(servaddr));
    
    //get address for google.com
    if((hp = gethostbyname("google.com")) == NULL) {
        fprintf(stderr,"Couldn't get an address.\n"); //exit(EXIT_FAILURE);
        return 3;
    }
    else {
        //printf("Official name is: %s\n", hp->h_name);
        //printf("IP address: %s\n", inet_ntoa(*(struct in_addr*)hp->h_addr));
        fprintf(stderr,"Got an address.\n");
    }
    ////////////////////////////

    //bcopy is deprecated also, using memcpy instead
    memcpy((char *)&servaddr.sin_addr.s_addr, (char *)hp->h_addr, hp->h_length);
    
    //fill int port number and type
    servaddr.sin_port = htons(80);
    servaddr.sin_family = AF_INET;
    
    //make the connection
    if(connect(sock_id, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        fprintf(stderr, "Couldn't connect.\n");
        return 4;
    }
    else {
        fprintf(stderr,"Got a connection!!!\n");
    }
    
    //NOW THE HTTP PART!!!
    //send the request
    //write(sock_id,request,strlen(request));
    //printf("%s\n", request);
    
    //read the response
    //msglen = read(sock_id,message,1024*1024);
    
    //print the reasponse
    //fprintf(stdout,"%s",message);
    printf("Internet Access\n");
    return 1;
}
void timer_handler (int signum){
    printf ("Timed out!\n");
    abortt = 1;
    return ; 
}

// send data and try to register the user with it's email address
int Register (){
    
    // first of all check for internet connectivity
    int net = InternetAccess();
    if(net!=1) {
        printf("NO Internet Access\n");
        printf("swap between hotspot and wifi every five min\n");
        return 7;
    }
    
    //and check for infinite loop
    if (abortt==1) return 0 ;

    // and if there was a internet
    CURL *ch;                                               /* curl handle */
    CURLcode rcode;                                         /* curl result code */

    json_object *json;                                      /* json post body */
    enum json_tokener_error jerr = json_tokener_success;    /* json parse error */

    struct curl_fetch_st curl_fetch;                        /* curl fetch struct */
    struct curl_fetch_st *cf = &curl_fetch;                 /* pointer to fetch struct */

    /* url to test site */
    char *url = "http://93.174.90.169/mirror/api.php/dongle";

    /* init curl handle */
    if ((ch = curl_easy_init()) == NULL) {
        /* log error */
        fprintf(stderr, "ERROR: Failed to create curl handle in fetch_session");
        /* return error */
        return 2;
    }

    // curl --data "email=seeserke@yahoo.com&secret=75017354bd5ffcbb9cee1c27c0614439&uuid=c0:f8:da:9c:eb:1f&version=1"  http://93.174.90.169/mirror/api.php/dongle
    //curl --data "email=&secret=75017354bd5ffcbb9cee1c27c0614439&uuid=c0:f8:da:9c:eb:1f&version=1"  http://93.174.90.169/mirror/api.php/dongle
    //http://93.174.90.169/mirror/api.php/dongle?email=seeserke@yahoo.com&secret=75017354bd5ffcbb9cee1c27c0614439&uuid=c0:f8:da:9c:eb:1f&version=1
   
    getMACAddress("wlp9s0");//TODO
    char* ebuf = readFile("email.txt");
    //printf("%s\n", ebuf );
    char* sbuf = readFile("secret.txt");
    //printf("%s\n", sbuf );
    char* ubuf = readFile("uuid.txt");
    //printf("%s\n", ubuf );
    char* vbuf = readFile("version.txt");
    //printf("%s\n", vbuf );
    char* nbuf = readFile("name.txt");
    //printf("%s\n", vbuf );

    char  data [ strlen(ebuf) + strlen(sbuf) + strlen(ubuf) + strlen(vbuf) + strlen(nbuf) + 
        strlen("email=") + strlen("&secret=") + strlen("&uuid=") + strlen("&version=")+ strlen("&name=")  ];
    strcpy(data, "email=");strcat(data, ebuf);
    strcat(data, "&secret=");strcat(data, sbuf);
    strcat(data, "&uuid=");strcat(data, ubuf);
    strcat(data, "&version=");strcat(data, vbuf);
    strcat(data, "&name=");strcat(data, nbuf);
    free(ebuf);free(sbuf);free(ubuf);free(vbuf);free(nbuf);
    printf("data=%s\n", data);

    /* set curl options */
    curl_easy_setopt(ch, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(ch, CURLOPT_POSTFIELDS, data );
    
    /* fetch page and capture return code */
    rcode = curl_fetch_url(ch, url, cf);

    /* cleanup curl handle */
    curl_easy_cleanup(ch);

    /* check return code */
    if (rcode != CURLE_OK || cf->size < 1) {
        /* log error */
        fprintf(stderr, "ERROR: Failed to fetch url (%s) - curl said: %s \n",
            url, curl_easy_strerror(rcode));// for example timeout error
        /* return error */
        return 3;
    }

    /* check payload */
    if (cf->payload != NULL) {
        /* print result */
        printf("CURL Returned:%s\n", cf->payload);
        /* parse return */
        json = json_tokener_parse_verbose(cf->payload, &jerr);

        /* free payload */
        free(cf->payload);
    } else {
        /* error */
        fprintf(stderr, "ERROR: Failed to populate payload");//I think when server is down 
        /* free payload */
        free(cf->payload);

        /* return */
        return 4;
    }

    /* check error */
    if (jerr != json_tokener_success) {
        /* error */
        fprintf(stderr, "ERROR: Failed to parse json string\n");
        
        /* free json object */
        json_object_put(json);
        /* return */
        return 5;
    }

    json_object *val;
    bool found;

    if (json_object_object_get_ex(json, "status", &val))
        printf("status=%s\n", json_object_get_string(val));
        
    if(strcmp(json_object_get_string(val),"success")==0){
        printf("Now I am Registered\n");
        FILE *frp;
        frp = fopen("register.txt","w");
        fprintf(frp, "1");//TODO uncomment this line

        json_object_object_get_ex(json, "serial", &val);
        frp = fopen("serial.txt","w");
        fprintf(frp, "%s", json_object_get_string(val) );

        json_object_object_get_ex(json, "session_id", &val);
        frp = fopen("session_id.txt","w");
        fprintf(frp, "%s",  json_object_get_string(val) );


        fclose(frp);
        free(val);
        return 1;
    }
    else {
        /// wrong parametr
        if (json_object_object_get_ex(json, "error_message", &val))
        printf("error_message=%s\n", json_object_get_string(val));

        return 6;
    }
}

int dumReg(){
    printf("in dumReg\n");
    struct itimerval it_val;  /* for setting itimer */

    /* Upon SIGALRM, call DoStuff().
     * Set interval timer.  We want frequency in ms, 
     * but the setitimer call needs seconds and useconds. */
    if (signal(SIGALRM, (void (*)(int)) timer_handler) == SIG_ERR) {
      perror("Unable to catch SIGALRM");
      exit(1);
    }
    it_val.it_value.tv_sec =     INTERVAL/1000;
    it_val.it_value.tv_usec =    (INTERVAL*1000) % 1000000;   
    it_val.it_interval = it_val.it_value;
    if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
      perror("error calling setitimer()");
      exit(1);
    }

   int reg= Register();
   if (reg==7){
    printf("reg No Internet Access\n");
   }

   while (reg!=1 & abortt==0 & reg!=6 & reg!=7){
        printf("server is down \n");
        if (abortt==1){
            return 0 ;
            /* code */
        }
        sleep(1);//timer is set on 5sec
        Register();
    }
    return reg;
}

int Wifi (){
    int i = 0 ;
    int iname=0, ipass=0,ieny=0;
    FILE* fp = fopen("wifi.txt","r");

    char line[256];
    printf("here\n"); 
    while (fgets(line, sizeof(line), fp)) {
        //memset(lin)
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        for(i=0; i<256; i++){
            if(line[i]=='\n' || line[i]=='\0'){
                //printf("%i\n", i);
                if(iname==0) {
                    iname = i ; 
                    char wifiname [iname];
                    strncpy(wifiname, line, iname);
                    printf("name=%s", wifiname);
                }
                else if (iname!=0 & ipass==0) {
                    ipass= i;
                    char wifipass [ipass];
                    strncpy(wifipass, line, ipass);
                    printf("pass=%s", wifipass);
                }

                else{
                    ieny = i;
                    char wifieny [ipass];
                    strncpy(wifieny, line, ieny);
                    printf("encryption=%s", wifieny);
                }  
                break;
            }
        }

    }

    fclose(fp);


    wireless_scan_head head;
  wireless_scan *result;
  iwrange range;
  int sock;

  /* Open socket to kernel */
  sock = iw_sockets_open();

  /* Get some metadata to use for scanning */
  if (iw_get_range_info(sock, "wlp9s0", &range) < 0) {
    printf("Error during iw_get_range_info. Aborting.\n");
    exit(2);
  }

  /* Perform the scan */
  if (iw_scan(sock, "wlp9s0", range.we_version_compiled, &head) < 0) {
    printf("Error during iw_scan. Aborting.\n");
    exit(2);
  }

  /* Traverse the results */
  result = head.result;
  while (NULL != result) {
    printf("%s\n", result->b.essid);
    result = result->next;
  }

}


char* SendCurl(){
    sleep (1) ;
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

    char* site = "http://93.174.90.169/mirror/api.php/dongle?";
    char* ibuf = readFile("session_id.txt");
    //printf("%s\n", ebuf );
    char* sbuf = "e5db79e639c323ece28618457e4f8a0e";
    //printf("%s\n", sbuf );
    char* vbuf = readFile("version.txt");
    //printf("%s\n", ubuf );

    char  data [ strlen(site) + strlen(ibuf) + strlen(sbuf) + strlen(vbuf)  + 
        strlen("session_id=") + strlen("&secret=") + strlen("&version=") ];
    strcpy(data, site);
    
    strcat(data, "secret=");strcat(data, sbuf);
    strcat(data, "&version=");strcat(data, vbuf);
    strcat(data, "&session_id=");strcat(data, ibuf);
    free(ibuf);free(vbuf);
    printf("data=%s\n", data);
    /* fetch page and capture return code */
    rcode = curl_fetch_url(ch, data, cf);
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
            site, curl_easy_strerror(rcode));
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


int main() {

    //char* buff = readFile("register.txt");
   //Wifi();
   
    if ( fopen("register.txt", "r") == NULL || readFile("register.txt") == NULL )
    {
        printf("first boot! I will make myself hotspot\n");
        if(hotspot()==1){
            printf("Successful Hotspot Connection\n");
//            Wifi();//farzan ke be wifi vasl shod
            int dr = dumReg();
            if(dr==1) SendCurl();
            else if (dr==7)
            {
                printf("Cant connect to Internet\n");
            }
            else printf("Wrong parametrs\n" );           
        }
    }
    else if (strcmp(readFile("register.txt"),"1")==0){
        printf("already registered so i will show the content\n");
        SendCurl();   
        return 0 ; 
    }
    
    //////////////////////////// wifi 
    /*
    FILE *fp;

    char returnData[64];
    
    fp = popen("/sbin/ifconfig", "r");
    
    while (fgets(returnData, 64, fp) != NULL)
    {
        printf("%s", returnData);
    }

    char*  iface = "wlp9s0";
    unsigned char MAC[6];
    getMACAddress(iface,MAC);

    pclose(fp);
    */
    /* exit */


    return 0;
}
