/*
    TC4-WB for Artisan Coffee Roaster Application

    Released under BSD-NC-3.0 License

    Created by Sakunamary on 2022

    define and web page raw data 
*/    

#ifndef __TC4_H__
#define __TC4_H__

#define TEMPERATURE_ARRAY_LENGTH    4
#define TEMPERATURE_ROR_LENGTH      6


#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define BAUDRATE 115200  //serial port baudrate

#define  FULL_VERSION         //full function version ,with wifi and bluetooth
//#define  WIFI_VERSION       //only wifi version 
//#define  BLUETOOTH_VERSION  //only bluetooth version

#if defined(FULL_VERSION)
    #define VERSION "1.1.7f"
#endif    
#if defined(WIFI_VERSION)
    #define VERSION "1.1.7w"
#endif 
#if defined(BLUETOOTH_VERSION)
    #define VERSION "1.1.7b"
#endif



// 网页设置的参数
 typedef struct eeprom_settings 
{
  char ssid[60]; //增加到30个字符
  char password[60]; //增加到30个字符
  float  btemp_fix;
  float  etemp_fix;
  double sampling_time;//采样时间   单位：s
  int    sleeping_time ;//休眠时间  单位：s
  bool   Init_mode ; //是否初始化模式
} user_wifi_t;

extern user_wifi_t  user_wifi ;

//温度数据数组

typedef struct temp_data
{
    float BT_AvgTemp ; 
    float ET_AvgTemp ;
    float BT_ROR ;
    float ET_ROR ;
} temperature_data_t;

extern temperature_data_t temperature_data;



//SemaphoreHandle_t xThermoDataMutex = NULL;

////////////////////////////////////////////////////////////////
//
//  web page raw data 
//
////////////////////////////////////////////////////////////////
const char wifi_sussce_html[] PROGMEM = R"rawliteral(
<!doctype html><html lang='cn'><head>
    <meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'>
        <title>TC4-WB Wifi Setup</title>
          <style>*,::after,::before{box-sizing:border-box;}
    body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';
    font-size:1rem;
    font-weight:400;
    line-height:1.5;color:rgb(113, 58, 19);
    background-color:rgb(252, 231, 174);}
    .form-control{
    display:block;
    width: 400px;
    height:calc(1.5em + .75rem + 2px);
    border:1px solid #ced4da;}
    button{border:1px solid transparent;
    color:#fff;
    background-color:rgb(122, 72, 23);
    border-color:rgb(113, 59, 18);
    padding:.5rem 1rem;
    font-size:1.25rem;
    line-height:1.5;
    border-radius:.3rem;
    width:400px}
    .form-signin{
    width: 400px;
    padding:15px;
    margin:auto;}
    h1,p{text-align:center}</style> 
        </head> 
<body>
    <main class='form-signin'> 
        <h1>TC4-WB 设置成功</h1> <br/> 
        <p>
        请手动重启生效<br />
        如不成功请重复操作<br />
        </p>
    </main>
</body></html>
)rawliteral";


const char index_html[] PROGMEM = R"rawliteral(
<!doctype html><html lang='cn'>
<head>
<script>
  function submitMessage() {
    alert("数据已保存");
    setTimeout(function(){ document.location.reload(false); }, 500);
  }
</script>
    <meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'>
    <title>TC4-WB 设置</title>
    <style>*,::after,::before{box-sizing:border-box;}
    body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';
    font-size:1rem;
    font-weight:400;
    line-height:1.5;color:rgb(113, 58, 19);
    background-color:rgb(252, 231, 174);}
    .form-control{
    display:block;
    width: 400px;
    height:calc(1.5em + .75rem + 2px);
    border:1px solid #ced4da;}
    button{border:1px solid transparent;
    color:#fff;
    background-color:rgb(122, 72, 23);
    border-color:rgb(113, 59, 18);
    padding:.5rem 1rem;
    font-size:1.25rem;
    line-height:1.5;
    border-radius:.3rem;
    width:150px;
   }
    .form-signin{
    width: 400px;
    padding:15px;
    margin:auto;
    }
    h1,p{text-align:center}
    </style> 
</head> 
<body>
    <main class='form-signin'> 
        <form action='/get' method='get'>
            <h1 class=''>TC4-WB 设置</h1>
            <h2 class=''>1.WIFI 设置</h2>
            <div class='form-floating'>
            <label>SSID/WIFI名字</label>
            <input type='text' class='form-control' name='ssid'> 
            </div>
            <div class='form-floating'>
            <br/>
            <label>PASSWORD</label>
            <input type='password' class='form-control' name='password'>
            </div>
            <p>
            提示:输入空白即恢复AP模式直链模式
            </p>
            <br/>
            <div align="center">
                <button  type='submit'>保存</button>
            </div>
            
        </form>     
        <form action='/compens' method='get'>                
            <br/>
            <br/>
            <h2 class=''>2.电偶温度补偿设置</h2>
            <div class='form-floating'>
            <label>Bean Temp/豆温 (current: %bt_compens%) </label>
            <input type='number' step = '0.01' max = '20' min='-20' class='form-control'  name='Btemp_fix'> 
            </div>
            <br/>
            <div class='form-floating'>
            <label>Env  Temp/炉温 (current:%et_compens%)</label>
            <input type='number' step = '0.01' max = '20' min='-20' class='form-control' name='Etemp_fix'> 
            </div>
            <br/>
            <div align="center">
                <button  type='submit'onclick="submitMessage()">保存</button>
            </div>
            
        </form> 
        <form action='/other' method='get'>   
            <br/>
            <br/>
            <div class='form-floating'>
            <h2 class=''>3.杂项</h2>  
            <label>温度采样时间 (current: %sampling_time%) s</label>
            <input type='number' step = '0.25' max = '4' min='0.75' class='form-control'  name='sampling_time'> 
            </div>
            <br/>
            <div class='form-floating'>
            <label>休眠时间 (current:%sleeping_time%) Mins</label>
            <input type='number' step = '1' max = '30' min='5' class='form-control' name='sleeping_time'> 
            </div>
            <br/>
            <div align="center">
                <button type='submit'onclick="submitMessage()">保存</button>
            </div>
            
        </form> 
            <p>
            <a href='/update' target='_blank'>FIRMWARE UPDATE verison:%version%</a>
            </p>
            <br/>
    </main> 
</body></html>
)rawliteral";

const char update_html[] PROGMEM = R"rawliteral(
<!doctype html><html lang='cn'>
<head>
    <meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'>
    <title>TC4-WB Setup</title>
    <style>*,::after,::before{box-sizing:border-box;}
    body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';
    font-size:1rem;
    font-weight:400;
    line-height:1.5;color:rgb(113, 58, 19);
    background-color:rgb(252, 231, 174);}
    .form-control{
    display:block;
    width: 400px;
    height:calc(1.5em + .75rem + 2px);
    border:1px solid #ced4da;}
    button{border:1px solid transparent;
    color:#fff;
    background-color:rgb(122, 72, 23);
    border-color:rgb(113, 59, 18);
    padding:.5rem 1rem;
    font-size:1.25rem;
    line-height:1.5;
    border-radius:.3rem;
    width:400px}
    .form-signin{
    width: 400px;
    padding:15px;
    font-size:1rem;
    margin:auto;}
    h1,p{text-align:center}
    .input_control{
    color:#fff;
    background-color:rgb(122, 72, 23);
    border-color:rgb(113, 59, 18);
    padding:.5rem 1rem;
    font-size:1.25rem;
    line-height:1.5;
    border-radius:.3rem;
    align-items: center;
    width:150px   
    }
    </style> 
</head> 
<body>
    <main class='form-signin'> 
        <h1 class=''>固件升级</h1>
        <form method='POST' action='/update' enctype='multipart/form-data'>
         <div class='form-floating'>               
        <H2>选择固件文件</H2>
        <input type='file' name='update'class='form-signin'>
    </div> 
        <br/>
        <input type='submit' value='升级' class='input_control' >
   
    </form>
    </main> 
</body></html>
)rawliteral";



const char update_OK_html[] PROGMEM = R"rawliteral(
<!doctype html><html lang='cn'><head>
    <meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'>
        <title>固件升级成功</title>
        <style>*,::after,::before{box-sizing:border-box;}
            body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';
            font-size:1rem;
            font-weight:400;
            line-height:1.5;color:rgb(113, 58, 19);
            background-color:rgb(252, 231, 174);}
            .form-control{
            display:block;
            width: 400px;
            height:calc(1.5em + .75rem + 2px);
            border:1px solid #ced4da;}
            button{border:1px solid transparent;
            color:#fff;
            background-color:rgb(122, 72, 23);
            border-color:rgb(113, 59, 18);
            padding:.5rem 1rem;
            font-size:1.25rem;
            line-height:1.5;
            border-radius:.3rem;
            width:400px}
            .form-signin{
            width: 400px;
            padding:15px;
            margin:auto;}
            h1,p{text-align:center}</style> 
        </head> 
<body>
    <main class='form-signin'> 
        <h1>固件升级成功</h1> <br/> 
        </p>
        <p>设置成功<br />
        模块将在3秒后重启<br />
        </p>
    </main>
</body></html>
)rawliteral";


const char update_fail_html[] PROGMEM = R"rawliteral(
<!doctype html><html lang='cn'><head>
    <meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'>
        <title>固件升级成功</title>
        <style>*,::after,::before{box-sizing:border-box;}
            body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';
            font-size:1rem;
            font-weight:400;
            line-height:1.5;color:rgb(113, 58, 19);
            background-color:rgb(252, 231, 174);}
            .form-control{
            display:block;
            width: 400px;
            height:calc(1.5em + .75rem + 2px);
            border:1px solid #ced4da;}
            button{border:1px solid transparent;
            color:#fff;
            background-color:rgb(122, 72, 23);
            border-color:rgb(113, 59, 18);
            padding:.5rem 1rem;
            font-size:1.25rem;
            line-height:1.5;
            border-radius:.3rem;
            width:400px}
            .form-signin{
            width: 400px;
            padding:15px;
            margin:auto;}
            h1,p{text-align:center}</style> 
        </head> 
<body>
    <main class='form-signin'> 
        <h1>固件升级失败</h1> <br/> 
        </p>
        <p>模块将在3秒后重启<br />
        重启后请重新上传<br />
        </p>
    </main>
</body></html>
)rawliteral";



#endif /*__TC4_H__*/
