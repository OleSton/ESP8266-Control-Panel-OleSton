
		var ProgNum = '0';
		var Tempr	= '0';
		var SpeedSpin= '0';
		var StartBotton = '0';
		var PowerBotton = '0';
    	var StatusWash  = '0';
    	var StopUpdate  =  1 ;
    	var ConnectStatus = 0;
        var delaySecShowNewTab = 0;
        var IPaddress = '';

    var flON = 0;
    var lockStartBt = 0;
    var lockSaveWIFIBt = 0;
    var images = new Array();
/*---------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------При первой загрузке (перезагрузке) страницы------------------------------*/
document.addEventListener("DOMContentLoaded",firstLoad ());
/*---------------------------------------------------------------------------------------------------------------*/

function StartBt(ON_OFF) {

  if (ON_OFF && StartBotton != '1') 
  {
     StartBotton = '1';
     SpeedSpin = $("[name=optionsS]:checked").val();
     Tempr = $("[name=options]:checked").val();
     ProgNum = $("#slider1").data("roundSlider").option("value");
   // alert(getURLParameters('tempr', "http://192.168.4.1/genericArgs?prog=15&speed=500&tempr=45&powerSW=0&startSW=1"));
    //alert (" START BOTTON To ON ");
  } 
  if (!ON_OFF) {
    //alert (" STOP BOTTON To OFF");
    StartBotton = '0';
  }
   setTimeout(sendDataToESP(), 1000);  //<<<<<<<<??
    return;
}
/*---------------------------------------------------------------------------------------------------------------*/
function sendDataToESP(){
StopUpdate = 0;
//tmpStartBT = 0;
var request = new XMLHttpRequest();
var param = '/genericArgs?prog=' + ProgNum + '&speed=' + SpeedSpin + '&tempr=' + Tempr + '&powerSW=' + PowerBotton + '&startSW=' + StartBotton ;
			request.open('GET', param, false); //<<<<<<<<<<<<<<<<<<<<
			request.onload = function() {
				if (request.readyState == 4 && request.status == 200) 
				{
					var response = request.responseText;
					  /*  tmpStartBT = getURLParameters('startSW', response);
						if (StartBotton == '1' && tmpStartBT == '1') {
                                    									$("#slider1").roundSlider("disable");}
                        if (StartBotton == '1' && tmpStartBT == '0') { 
                        												StartBotton = '0';
                        												btn.click();
                                            							$("#slider1").roundSlider("enable");}
                */	
				}
			}
			request.send();
StopUpdate = 0;
}
/*---------------------------------------------------------------------------------------------------------------*/
function getStatusESP(){
tmpStartBTe = 0;

var request = new XMLHttpRequest();
var param = '/getStatus?status';//1=1&prog=' + ProgNum + '&speed=' + SpeedSpin + '&tempr=' + Tempr + '&startSW=' + StartBotton;
			request.open('GET', param, true);
			request.onload = function() {
				if (request.readyState == 4 && request.status == 200) 
				{
				  var response = request.responseText;
				  //if (StartBotton == '1' && 
				  	if (StopUpdate > 3 )
					{
						tmpStartBTe = getURLParameters('startSW', response);
						ProgNum = getURLParameters('prog', response);
						SpeedSpin = getURLParameters('speed', response);	
						Tempr = getURLParameters('tempr', response);
						document.getElementById("cur_tempr").innerHTML = getURLParameters('cTemp', response) + '°';
          				document.getElementById("cur_spin").innerHTML = getURLParameters('cSpeed', response);
                        IPaddress = getURLParameters('IP', response);
					//if (StartBotton == '1' && 
						if (tmpStartBTe == '1' && active == false) {
                                    								  //active = true;
                                    								  lock_start_bt(1);
                        											  StartBotton = '1';
                        											  btn.click();
                        											  $("#slider1").data("roundSlider").option("value", ProgNum);
																	  document.getElementById("img_main").src = images[ProgNum];
																	  $('input[name="options"][value="'+Tempr+'"]').prop('checked', true);
																	  $('input[name="optionsS"][value="'+SpeedSpin+'"]').prop('checked', true);
                                    								}
                    //if (StartBotton == '1' && 
                    	if (tmpStartBTe == '0' && active == true) { 
                        												//active = false;
                        												lock_start_bt(1);
                        												StartBotton = '0';
                        												btn.click();
                                            							//$("#slider1").roundSlider("enable");
                                            						}
                    
					}

          statusWash = getURLParameters('statusWash', response);
          $("#statusPic").attr("href", "#St" + statusWash);

					document.getElementById("StatusWash").innerHTML = 
					'Status ' + statusWash + 
					'<br>Sec pause ' + getURLParameters('cSecPause', response) + '<br>Set tempr ' + getURLParameters('cTempSet', response) +
					'<br>Curent prog ' + getURLParameters('cNumProg', response) +
					'<br>Status door ' + getURLParameters('cStatusDoor', response) + '<br>Water level ' + getURLParameters('cWaterLevel', response) +
					'<br>Count Start ' + getURLParameters('CountStart', response) +
					'<br>============='+
					'<br>Start Botton ' + getURLParameters('startSW', response) + '<br>Prog Num ' + getURLParameters('cNumProg', response) +
					'<br>Tempr  ' + getURLParameters('cTemp', response) + '°' + '<br>Speed Spin ' + getURLParameters('cSpeed', response) +
					'<br>IP   ' + IPaddress;

         			

					ConnectStatus = 5;
					


				} 
			}
			request.send();	
			StopUpdate++;
			ConnectStatus--;
			if (ConnectStatus > 0)
								{//document.getElementById("conn").src = 'connect.png';
                $("#connectWiFiPic").attr("href", "#picWiFiConnect");
								}else 
									{//document.getElementById("conn").src = 'disconnect.png';
                  $("#connectWiFiPic").attr("href", "#picWiFiUnConnect");
									}
}
/*---------------------------------------------------------------------------------------------------------------*/
function getWiFiList (){
$('#ssid_name').empty();
var request = new XMLHttpRequest();
var param = '/getWiFiList?list=';
			request.open('GET', param, true); //<<<<<<<<<<<<<<<<<<<<
			request.onload = function() {
				if (request.readyState == 4 && request.status == 200) 
				{
					var station = jQuery.parseJSON(request.responseText);

					for (key in station) 
					{
 					 if (station.hasOwnProperty(key)) 
 					 	{
   						 	console.log("Ключ = " + key);//ключ = key
    						console.log("Значение = " + station[key]);//значение = station[key] //$('#List_station').append('<a  href="#" onclick="selectWiFiStation(\''+key+'\')">'+ key +'</a>');
  						if (key != "modeWIFI" || key != "IPSTA" || key != "IPAP")
                                                                                 $('#ssid_name').append('<option>'+ key +'</option>');
                        } // если объект station имеет key (если у station есть свойство key)
					} // перерабрать все ключи (свойства) в объекте
                   linkIPSTA =  "&nbsp<a id=\"link1\" class=\"link\" href=\"http:\/\/" + station["IPSTA"]+"\">"+ "IP STA: " + station["IPSTA"] + "</a>";
                   linkAP    =  "&nbsp<a id=\"link2\" class=\"link\" href=\"http:\/\/" + station["IPAP"]+"\">"+ " IP AP: " + station["IPAP"] + "</a>";
                    document.getElementById("WIFIinfo").innerHTML = linkIPSTA  +  linkAP;
	               $("#WIFImode").val(station["modeWIFI"]);
                   changePicFiFiMode(station["modeWIFI"]);
                   //setTimeout(getWiFiList, 10000, 0);
                   //setTimeout(OpenNewTab,  20000, 0);
				}
			}
			request.send();
}
/*---------------------------------------------------------------------------------------------------------------*/
function OpenNewTab(ipLink){
    delaySecShowNewTab++;
    document.getElementById("countLoadNewTab").innerHTML = 11 - delaySecShowNewTab;
    if (delaySecShowNewTab > 10) { 
                                   window.open("http://" + ipLink); 
                                   delaySecShowNewTab = 0; 
                                   getWiFiList ();
                                   document.getElementById("countLoadNewTab").innerHTML = "<a id=\"link3\" class=\"link3\"target=\"_blank\" href=\"http:\/\/" + IPaddress+"\">"+ "Clik here to go new Tab " + IPaddress + "</a>";
                                   return;}
    setTimeout(OpenNewTab,  1000, IPaddress); 
}
/*---------------------------------------------------------------------------------------------------------------*/
function saveWiFiConfig(){
   // setTimeout(OpenNewTab,  1000, IPaddress); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<,
  if (lockSaveWIFIBt == 0) {$('#lockedWi').attr('class', 'lock_shake'); return;}
  document.getElementById("countLoadNewTab").innerHTML = "One moment...";
  //document.getElementById("lockSave").src = './lockOFF.png'; 
  $("#lockedWi").css('display', 'flex'); $("#unlockWi").css('display', 'none');
  lockSaveWIFIBt = 0;
	pass = $('#ssid_pass').val();
	ssid = $('#ssid_name option:selected').text();//$('#ssid_name').val();
	mode = $("#WIFImode").val();
   if (pass.length < 8 || ssid > 1 && mode == "2") {document.getElementById("countLoadNewTab").innerHTML = "SSID name or Password must be > 7 characters!!!"; return;}
   //ipSTA = "";
	var request = new XMLHttpRequest();
	var param = '/getConnectWiFi?ssidName=' + ssid + '&ssidPass=' + pass + '&modeWiFi=' + mode;
			request.open('GET', param, true); 
			request.onload = function() {
				if (request.readyState == 4 && request.status == 200) 
				{
					var response = request.responseText;
					ip = getURLParameters('IP', response);
					console.log("IP STA : " + ip);
                    setTimeout(OpenNewTab,  1000*3, IPaddress);
				}
			}
			request.send();

}
/*---------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------------*/

function firstLoad ()
{
$("#connectWiFiPic").attr("href", "#picWiFiUnConnect");
$("#locked").css('display', 'flex'); $("#unlock").css('display', 'none');
$("#lockedWi").css('display', 'flex'); $("#unlockWi").css('display', 'none');
var request = new XMLHttpRequest();
var param = '/getStatus?status';
			request.open('GET', param, true);
			request.onload = function() {
				if (request.readyState == 4 && request.status == 200) {
					var response = request.responseText;

					tmpStartBTe = getURLParameters('startSW', response);
					ProgNum = getURLParameters('prog', response);
					SpeedSpin = getURLParameters('speed', response);
					Tempr = getURLParameters('tempr', response);
					document.getElementById("cur_tempr").innerHTML = Tempr + '°';
					document.getElementById("cur_spin").innerHTML = SpeedSpin;

					$("#slider1").data("roundSlider").option("value", ProgNum);
					 $("#progPicW").attr("href", "#p"+ProgNum);
          //document.getElementById("img_main").src = images[ProgNum];


					$('input[name="options"][value="'+Tempr+'"]').prop('checked', true);
					$('input[name="optionsS"][value="'+SpeedSpin+'"]').prop('checked', true);
				/*		
					//if (StartBotton == '1' && 
						if (tmpStartBTe == '1' && active == false) {
                                    								  //active = true;
                                    								  lock_start_bt(1);
                        											  StartBotton = '1';
                        											  btn.click();
                                    								}
                    //if (StartBotton == '1' && 
                    	if (tmpStartBTe == '0' && active == true) { 
                        												//active = false;
                        												lock_start_bt(1);
                        												StartBotton = '0';
                        												btn.click();
                                            							//$("#slider1").roundSlider("enable");
                                            						}
*/
				}
			}
			request.send();		
	
StopUpdate = 2; 
getWiFiList (); 
setInterval(getStatusESP, 1000*1);
}
/*---------------------------------------------------------------------------------------------------------------*/

function getURLParameters(paramName, sURL)
{
    //var sURL = window.document.URL.toString();
    if (sURL.indexOf("?") > 0)
    {
        var arrParams = sURL.split("?");
        var arrURLParams = arrParams[1].split("&");
        var arrParamNames = new Array(arrURLParams.length);
        var arrParamValues = new Array(arrURLParams.length);

        var i = 0;
        for (i = 0; i<arrURLParams.length; i++)
        {
            var sParam =  arrURLParams[i].split("=");
            arrParamNames[i] = sParam[0];
            if (sParam[1] != "")
                arrParamValues[i] = unescape(sParam[1]);
            else
                arrParamValues[i] = "No Value";
        }

        for (i=0; i<arrURLParams.length; i++)
        {
            if (arrParamNames[i] == paramName)
            {
                //alert("Parameter:" + arrParamValues[i]);
                return arrParamValues[i];
            }
        }
        return "No Parameters Found";
    }
}
/*---------------------------Таймер выполнения функции-------------------------------------------------------------*/
//setInterval(getStatusESP, 1000*1); удалить