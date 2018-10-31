#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/system_properties.h>


// 读手机设备的IMSI号码（15位char string）
// method 1: from system properties
// method 2: execute "dumpsys iphonesubinfo" command
// This code uses 2 methods, should work on most single-sim GSM/WCDMA/LTE phones.

// Update: The idea of run dumpsys will not work,
// because dumpsys needs android.permission.DUMP and
// Android no longer allows non-system apps to have that permission.


// Android设备ID目前没有好的native code可以得到，但可以通过java method取到.

// 利用popen执行命令行获得输出，结果用keyword过滤每一行，如有满足则返回该行
int exec_command(const char* cmd, const char* keyword, char* result)
{
  FILE* pipe = popen(cmd, "r");
  if (!pipe)
    return 0;

  char line[256] = {0};
  int len = 0;
  while (!feof(pipe))
  {
    if (fgets(line, sizeof(line), pipe) != NULL)
    {
      // parse this line
      if ( strstr(line, keyword) != 0 )
      {
        // found
        len = strlen(line);
        strcpy(result, line);
        break;
      }
    }
  }
  pclose(pipe);
  return len;
}





int get_android_imei(char* buffer)
{
  char imei[100]={0};

  //returns the string length of the value.
  int ir = __system_property_get("ro.gsm.imei", imei);
  if (ir > 0)
  {
    imei[15] = 0; //strz end
    printf("got imei from system propety\n");
  }
  else
  {
    printf("get imei from system property failed\n");
    printf("trying dumpsys command\n");
    //old dumpsys imei getter
/* output sample:
root@vbox86p:/data/tmp # dumpsys iphonesubinfo                                 
Phone Subscriber Info:
  Phone Type = GSM
  Device ID = 000000000000000
*/

    char line[256] ={0};
    const char* imei_start_match = "Device ID = ";
    int imei_start_match_len = strlen(imei_start_match);
    int len = exec_command("dumpsys iphonesubinfo", imei_start_match, line);
    if (len == 0)
    {
      return 0;
    }

    char* imei_start = strstr(line, imei_start_match);
    if (imei_start && strlen(imei_start) >= imei_start_match_len + 15)
    {
      imei_start += imei_start_match_len;
      imei_start[15] = 0;
      printf("dumpsys works.\n");
      strcpy(imei, imei_start);
    }
  }

  //printf("imei=%s\n", imei);
  strcpy(buffer, imei);
  return 0;
}


