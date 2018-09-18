
#include <netdb.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cJSON.h"



#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//saweather.market.alicloudapi.com/spot-to-weather

void show_weather_info(char *json);
 
 int main(int argc, char **argv)
{

    char   *ptr, **pptr;
    struct hostent *hptr;
    char   str[200];
	char *a,b[100];


    ptr = "saweather.market.alicloudapi.com";
 
     if((hptr = gethostbyname(ptr)) == NULL)
    {
         printf(" gethostbyname error for host:%s\n", ptr);
         return 0;
     }


     printf("official hostname:%s\n",hptr->h_name);
     for(pptr = hptr->h_aliases; *pptr != NULL; pptr++)
     printf(" alias:%s\n",*pptr);
	 

     switch(hptr->h_addrtype)
     {
         case AF_INET:
         case AF_INET6:
             pptr=hptr->h_addr_list;
             for(; *pptr!=NULL; pptr++)
                 printf(" address:%s\n",
                        (char*)inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));		
						
             printf(" first address: %s\n",
                        (char*)inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str)));
         break;
         default:
             printf("unknown address type\n");
         break;
     }
	 
 
	 
	 printf("IPAddress:%s",(char*)inet_ntoa(*((struct in_addr*)hptr->h_addr)));


	
	


	 int fd = socket(AF_INET, SOCK_STREAM, 0);
	
	
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	
	addr.sin_addr.s_addr = inet_addr((char*)inet_ntoa(*((struct in_addr*)hptr->h_addr)));
	//addr.sin_addr.s_addr = inet_addr(argv[1]);
	addr.sin_port = htons(80);
	int cone = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
	
	if (cone == 0)
	{
		printf("connect is  OK! \n");
	
	}
	
	char *buf = calloc(1, 500);
	char *recvbuf = calloc(1,6000);
	int m = 0; 
	char phone_code[10];
  	
	printf("请输入区号：");
  	scanf("%s",phone_code);
	printf("%s",phone_code);
	bzero(buf, 500);
	snprintf(buf, 500, "GET /spot-to-weather?"
                            "area=%s "
                            "HTTP/1.1\r\n"
                            "Host:saweather.market.alicloudapi.com\r\n"
                            "Authorization:APPCODE 2f835887d28947ccb3f8d725ad3fe208\r\n\r\n",
                            phone_code);
	send(fd, buf, strlen(buf),0);


//'https://jisutianqi.market.alicloudapi.com/weather/city' 
//-H 'Authorization:APPCODE 你自己的AppCode'




	while(1)
	{
	
		int n = recv(fd,recvbuf,6000,0);
		if(n == 0)
		{
			break;
		}
		if(n == -1)
		{
			printf("recv is Eroor \n");
			exit(0);
		}
	
		

	//	printf("recv : %s", recvbuf);

		show_weather_info(strstr(recvbuf, "{"));

		free(recvbuf);
		free(buf);
		exit(0);
			
	}
	

	
	close(fd);
    return 0;
 }
 
 
 
 
 
 
 
 
 


 
  void show_weather_info(char *json) 
{
	
	
	cJSON *root     = cJSON_Parse(json);
	cJSON *body     = cJSON_GetObjectItem(root, "showapi_res_body");

	if(cJSON_GetObjectItem(body, "ret_code")->valueint == -1)
	{
		printf("%s\n",cJSON_GetObjectItem(body, "remark")->valuestring);	
		return;
	}

	cJSON *now      = cJSON_GetObjectItem(body, "now");
	cJSON *cityInfo = cJSON_GetObjectItem(body, "cityInfo");
	cJSON *today    = cJSON_GetObjectItem(body, "f1");
	cJSON *tomorrow = cJSON_GetObjectItem(body, "f2");
	cJSON *day_3rd  = cJSON_GetObjectItem(body, "f3");

	char *country = cJSON_GetObjectItem(cityInfo, "c9")->valuestring;
	char *province= cJSON_GetObjectItem(cityInfo, "c7")->valuestring;
	char *city    = cJSON_GetObjectItem(cityInfo, "c5")->valuestring;

	bool zhixiashi = !strcmp(city, province);
	printf("城市：%s·%s%s%s\n\n", country, province,
			zhixiashi ? "" : "·", zhixiashi ? "" : city);

	printf("现在天气：%s\n",    cJSON_GetObjectItem(now, "weather")->valuestring);
	printf("现在气温：%s°C\n\n",cJSON_GetObjectItem(now, "temperature")->valuestring);

	printf("明天天气：%s\n",   cJSON_GetObjectItem(tomorrow, "day_weather")->valuestring);
	printf("日间气温：%s°C\n", cJSON_GetObjectItem(tomorrow, "day_air_temperature")->valuestring);
	printf("夜间气温：%s°C\n\n", cJSON_GetObjectItem(tomorrow, "night_air_temperature")->valuestring);

	printf("后天天气：%s\n",     cJSON_GetObjectItem(day_3rd, "day_weather")->valuestring);
	printf("日间气温：%s°C\n",   cJSON_GetObjectItem(day_3rd, "day_air_temperature")->valuestring);
	printf("夜间气温：%s°C\n\n", cJSON_GetObjectItem(day_3rd, "night_air_temperature")->valuestring);
	
	
}
 
 
 
 
 
 
 
