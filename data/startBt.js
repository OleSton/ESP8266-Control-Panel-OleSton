


const btn = document.querySelector('.btn');
const heading = document.querySelector('.heading');


let active = false;



const turnOn = () => {
  btn.classList.add('active');
  heading.classList.add('active');
}

const turnOff = () => {
  btn.classList.remove('active');
  heading.classList.remove('active');
}

const toggleAnimation = () => {
  btn.classList.remove('animating');
  active ? turnOn() : turnOff();
};

function clickHandler() {
  if (lockStartBt == 0) {$('#locked').attr('class', 'lock_shake'); return;}
  active = !active;
  btn.classList.add('animating');
  if (active) { // switch ON
                $("#slider1").roundSlider("disable");
                $("input[name=options], input[name=optionsS]").prop("disabled", true);
                lock_start_bt(0); 
                StartBt(1);
                //alert(SpedSpin + " " + Tempr);
              } else 
                    { // switch OFF

                      $("#slider1").roundSlider("enable");
                      $("input[name=options], input[name=optionsS]").prop("disabled", false);
                      lock_start_bt(0); 
                      StartBt(0);
                      }
  
  btn.addEventListener('animationend', toggleAnimation);

}


btn.addEventListener('click', clickHandler);


if (flON ==1) {
                btn.click();
                $("#slider1").roundSlider("disable");
                } else {
                        $("#slider1").roundSlider("enable");

                        }


