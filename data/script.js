/*---------------------------------------------------------------------------------------------------------------*/
$('.container_switch').width($('.container_top').width());
/*---------------------------------------------------------------------------------------------------------------*/	

$("#slider1").roundSlider({

    width: 50,
    radius: 325,
    value: 0,
    max: "16",
    handleSize: "+60",
    lineCap: "round",
    circleShape: "pie",
    startAngle: 315,
    handleShape: "dot",
    editableTooltip: false,

    drag: function (args) {
        // handle the drag event here
    $("#progPicW").attr("href", "#p"+args.value);
    },
    change: function (args) {
        // handle the change event here


    }
});
/*---------------------------------------------------------------------------------------------------------------/
if(flON == 1) 
{
$("#slider1").data("roundSlider").option("value", 5);
$("#slider1").data("roundSlider").option("max", 25);
document.getElementById("img_main").src = images[5]; 
}
/*---------------------------------------------------------------------------------------------------------------*/
$('.lock').on('click', function(e) {
  // $('.lock').removeClass('lock_shake');
    $('#locked').attr('class', '');
  lock_start_bt(!lockStartBt); 
})
/*---------------------------------------------------------------------------------------------------------------*/
$('.lockSave').on('click', function(e) {
                                        //$('.lockSave').toggleClass('lock_shake');
  if (lockSaveWIFIBt == 0 ) {
                            // document.getElementById("lockSave").src = './lockON.png';
                            $("#unlockWi").css('display', 'flex'); $("#lockedWi").css('display', 'none');
                            lockSaveWIFIBt = 1;
                            $('#lockedWi').attr('class', '');// $('.lockSave').removeClass('lock_shake');
                            setTimeout(lock_start_bt, 10000, 0);
                      }else{
                             // document.getElementById("lockSave").src = './lockOFF.png';
                             $("#lockedWi").css('display', 'flex'); $("#unlockWi").css('display', 'none');
                             lockSaveWIFIBt = 0;
                      }
})
/*---------------------------------------------------------------------------------------------------------------*/
function lock_start_bt(ON_OFF){
if (ON_OFF) {
              lockStartBt = 1; 
              // document.getElementById("lock").src = './lockON.png';
              $("#unlock").css('display', 'flex'); $("#locked").css('display', 'none');
              setTimeout(lock_start_bt, 10000, 0);
          }else{
              lockStartBt = 0;
              // document.getElementById("lock").src = './lockOFF.png';
              $("#locked").css('display', 'flex'); $("#unlock").css('display', 'none');
            } 
// document.getElementById("lockSave").src = './lockOFF.png';
$("#lockedWi").css('display', 'flex'); $("#unlockWi").css('display', 'none');
lockSaveWIFIBt = 0;

}
/*---------------------------------------------------------------------------------------------------------------*/
$('.set, .close').on('click', function(e) {
  getWiFiList ();
  //changePicFiFiMode("0"); ///////////////
 // $("#WIFImode").val("0");
  $('.menu').toggleClass('menu_active');
  $('.content').toggleClass('content_active');
})
/*---------------------------------------------------------------------------------------------------------------*/
$('.tempr').on('click', function(e) {
  $('.box_pr_coverT').toggleClass('box_pr_cover_showT');

  if ( $('.box_pr_coverS').height() > 1){
   										$('.box_pr_coverS').toggleClass('box_pr_cover_showS');
   									    setTimeout(function () {
  												$('.cur_spin').removeClass('cur_spin_hide')},1100);}

   if($(".box_pr_coverT").hasClass("box_pr_cover_showT")) 
  										{$('.cur_tempr').toggleClass('cur_tempr_hide');}
  										else
  											{
  												setTimeout(function () {
  												$('.cur_tempr').removeClass('cur_tempr_hide')},1100);}
})
/*---------------------------------------------------------------------------------------------------------------*/
$('.spin').on('click', function(e) {
  $('.box_pr_coverS').toggleClass('box_pr_cover_showS');
  if ( $('.box_pr_coverT').height() > 1) {
   										$('.box_pr_coverT').toggleClass('box_pr_cover_showT');
   									setTimeout(function () {
  												$('.cur_tempr').removeClass('cur_tempr_hide')},1100);}

   if($(".box_pr_coverS").hasClass("box_pr_cover_showS")) 
  										{$('.cur_spin').toggleClass('cur_spin_hide');}
  										else
  											{
  												setTimeout(function () {
  												$('.cur_spin').removeClass('cur_spin_hide')},1100);}


 // $('.content').toggleClass('content_active');
})
/*---------------------------------------------------------------------------------------------------------------*/
$('#WIFImode').change(function() {
   changePicFiFiMode($("#WIFImode").val());
});

function changePicFiFiMode(mode){
  switch (mode)
   {
     case "0":
     $("#SSID_Name").css('display', 'none'); 
     // document.getElementById("WM").src = './WM.png';
     $("#modeWIFIpic").attr("href", "#wifiAP");
     break;
     case "1":
     $("#SSID_Name").css('display', 'flex'); 
     // document.getElementById("WM").src = './WR.png';
     $("#modeWIFIpic").attr("href", "#wifiSTA");
     break;
     case "2":
     // document.getElementById("WM").src = './reset.png';
     $("#modeWIFIpic").attr("href", "#wifiPreset");
     break;
     default:
   }
}
/*-------------------------------------- Размеры окна браузера изменены. ----------------------------------------*/
$(window).resize(function(){
	$('.container_switch').width($('.container_top').width());
  console.log("container_top width: " + $('.container_top').width() + " .container_switch width " + $('.container_switch').width());
 
 //$(".rs-tooltip-text").css('display', 'non'); 
if ($(window).height() < 730)
                             {$("#slider1").roundSlider("option", "showTooltip", "false");
                         }else{
                              $("#slider1").roundSlider("option", "showTooltip", "true");
                               }

});
/*---------------------------------------------------------------------------------------------------------------*/