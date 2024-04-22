//
//  HTML PAGE
//

const char PAGE_AdminGeneralSettings[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>General Settings</strong>
<hr>
<form action="" method="post">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
	<td align="right">Name of Device</td>
	<td><input type="text" id="devicename" name="devicename" value=""></td>
</tr>
<tr>
  <td align="left" colspan="2"><hr></td>
</tr>

<tr>
  <td align="right">Get Line Token</td>
  <td align="right">https://notify-bot.line.me/my/</td>
</tr>


<tr>
  <td align="left" colspan="2"><hr></td>
</tr>
<tr>
  <td align="right">Line Token1(32 char)</td>
  <td><input type="text" maxlength="32" id="linetoken1" name="linetoken1" value=""></td>
</tr>
<tr>
  <td align="right">Line Token2(11 char)</td>
  <td><input type="text" maxlength="32" id="linetoken2" name="linetoken2" value=""></td>
</tr>
<tr>
  <td align="left" colspan="2"><hr></td>
</tr>

<tr>
  <td align="left" colspan="2">Press switch 1 [1]</td>
</tr>
<tr>
  <td align="right"> Enabled:</td>
  <td><input type="checkbox" id="tonenabled" name="tonenabled"></td>
</tr>

<tr>
  <td align="right"> Time:</td>
  <td><input type="text" id="tonhour" name="tonhour" size="2" value="00">:<input type="text" id="tonminute" name="tonminute" size="2" value="00"></td>
</tr>
<tr>
  <td align="left" colspan="2">Press switch 1 [2]:</td>
<tr>
  <td align="right"> Enabled:</td>
  <td><input type="checkbox" id="toffenabled" name="toffenabled"></td>
</tr>
<tr>
  <td align="right"> Time:</td>
  <td><input type="text" id="toffhour" name="toffhour" size="2" value="00">:<input type="text" id="toffminute" name="toffminute" size="2" value="00"></td>
</tr>

<tr>
  <td align="left" colspan="2"><hr></td>
</tr>

<tr>
  <td align="left" colspan="2">Press switch 2 [1]</td>
</tr>
<tr>
  <td align="right"> Enabled:</td>
  <td><input type="checkbox" id="tonenabled2" name="tonenabled2"></td>
</tr>

<tr>
  <td align="right"> Time:</td>
  <td><input type="text" id="tonhour2" name="tonhour2" size="2" value="00">:<input type="text" id="tonminute2" name="tonminute2" size="2" value="00"></td>
</tr>
<tr>
  <td align="left" colspan="2">Press switch 2 [2]:</td>
<tr>
  <td align="right"> Enabled:</td>
  <td><input type="checkbox" id="toffenabled2" name="toffenabled2"></td>
</tr>
<tr>
  <td align="right"> Time:</td>
  <td><input align="center" type="text" id="toffhour2" name="toffhour2" size="2" value="00">:<input align="center" type="text" id="toffminute2" name="toffminute2" size="2" value="00"></td>
</tr>


<tr>
  <td align="left" colspan="2"><hr></td>
</tr>

<tr>
  <td align="left" colspan="2">Humidity Alarm</td>
</tr>
<tr>
  <td align="right"> Enabled:</td>
  <td><input type="checkbox" id="humidityenable" name="humidityenable"></td>
</tr>
<tr>
  <td align="right"> Humidity:</td>
  <td><input align="right" type="text" id="humidityvalue" name="humidityvalue" size="2" value="00"> %</td>
</tr>


<tr>
  <td align="left" colspan="2"><hr></td>
</tr>

<tr>
  <td align="left" colspan="2">Temperature Alarm</td>
</tr>
<tr>
  <td align="right"> Enabled:</td>
  <td><input type="checkbox" id="tempenable" name="tempenable"></td>
</tr>
<tr>
  <td align="right"> Temperature:</td>
  <td><input align="right" type="text" id="tempvalue" name="tempvalue" size="2" value="00"> °C</td>
</tr>





<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>

</table>
</form>
<script>

window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				setValues("/admin/generalvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


// Functions for this Page
void send_devicename_value_html()
{
		
	String values ="";
	values += "devicename|" + (String) config.DeviceName + "|div\n";
	server.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 
	
}

void send_general_html()
{
	
	if (server.args() > 0 )  // Save Settings
  {
    config.AutoTurnOn[1] = false;
    config.AutoTurnOff[1] = false;
    config.AutoTurnOn[2] = false;
    config.AutoTurnOff[2] = false;
    config.AutoHumidity = false;
		String temp = "";
		for ( uint8_t i = 0; i < server.args(); i++ ) {
			if (server.argName(i) == "devicename") config.DeviceName = urldecode(server.arg(i)); 
      if (server.argName(i) == "linetoken1") config.LineToken1 = urldecode(server.arg(i));
      if (server.argName(i) == "linetoken2") config.LineToken2 = urldecode(server.arg(i));
      if (server.argName(i) == "tonenabled") config.AutoTurnOn[0] = true; 
      if (server.argName(i) == "toffenabled") config.AutoTurnOff[0] = true;
      if (server.argName(i) == "tonhour") config.TurnOnHour[0] =  server.arg(i).toInt(); 
      if (server.argName(i) == "tonminute") config.TurnOnMinute[0] =  server.arg(i).toInt(); 
      if (server.argName(i) == "toffhour") config.TurnOffHour[0] =  server.arg(i).toInt(); 
      if (server.argName(i) == "toffminute") config.TurnOffMinute[0] =  server.arg(i).toInt(); 
      if (server.argName(i) == "tonenabled2") config.AutoTurnOn[1] = true; 
      if (server.argName(i) == "toffenabled2") config.AutoTurnOff[1] = true; 
      if (server.argName(i) == "tonhour2") config.TurnOnHour[1] =  server.arg(i).toInt(); 
      if (server.argName(i) == "tonminute2") config.TurnOnMinute[1] =  server.arg(i).toInt(); 
      if (server.argName(i) == "toffhour2") config.TurnOffHour[1] =  server.arg(i).toInt(); 
      if (server.argName(i) == "toffminute2") config.TurnOffMinute[1] =  server.arg(i).toInt();
      if (server.argName(i) == "humidityenable") config.AutoHumidity = true;
      if (server.argName(i) == "humidityvalue") config.Humidity = server.arg(i).toInt();
      if (server.argName(i) == "tempenable") config.AutoTemp = true;
      if (server.argName(i) == "tempvalue") config.Temp = server.arg(i).toInt();
		}
		WriteConfig();
		firstStart = true;
	}
	server.send ( 200, "text/html", PAGE_AdminGeneralSettings ); 
	Serial.println(__FUNCTION__); 
}

void send_general_configuration_values_html()
{
	String values ="";
	values += "devicename|" +  (String)  config.DeviceName +  "|input\n";
  values += "linetoken1|" + (String) config.LineToken1 + "|input\n";
  values += "linetoken2|" + (String) config.LineToken2 + "|input\n";
  values += "tonhour|" +  (String)  config.TurnOnHour[0] +  "|input\n";
  values += "tonminute|" +   (String) config.TurnOnMinute[0] +  "|input\n";
  values += "toffhour|" +  (String)  config.TurnOffHour[0] +  "|input\n";
  values += "toffminute|" +   (String)  config.TurnOffMinute[0] +  "|input\n";
  values += "toffenabled|" +  (String) (config.AutoTurnOff[0] ? "checked" : "") + "|chk\n";
  values += "tonenabled|" +  (String) (config.AutoTurnOn[0] ? "checked" : "") + "|chk\n";
 
  values += "tonhour2|" +  (String)  config.TurnOnHour[1] +  "|input\n";
  values += "tonminute2|" +   (String) config.TurnOnMinute[1] +  "|input\n";
  values += "toffhour2|" +  (String)  config.TurnOffHour[1] +  "|input\n";
  values += "toffminute2|" +   (String)  config.TurnOffMinute[1] +  "|input\n";
  values += "toffenabled2|" +  (String) (config.AutoTurnOff[1] ? "checked" : "") + "|chk\n";
  values += "tonenabled2|" +  (String) (config.AutoTurnOn[1] ? "checked" : "") + "|chk\n";
  
  values += "humidityenable|" +  (String) (config.AutoHumidity ? "checked" : "") + "|chk\n";
  values += "humidityvalue|" +  (String)  config.Humidity +  "|input\n";

  values += "tempenable|" +  (String) (config.AutoTemp ? "checked" : "") + "|chk\n";
  values += "tempvalue|" +  (String)  config.Temp +  "|input\n";
  
	server.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 
}
