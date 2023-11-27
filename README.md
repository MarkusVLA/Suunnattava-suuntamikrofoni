# Suunnattava suuntamikrofoni 

Suunnattava suuntamikrofoni on laite, jolla voidaan elektronisesti ohjata äänityksen suuntaa käännettävän suuntauksen avulla. Laite koostuu kahdesta piirikortista: Ensimmäinen piirikortti sisältää kuuden mikrofonin ryhmän, josta luetaan dataa. Toinen piirikortti hoitaa signaalinkäsittelyn ja laitteen ohjauksen ESP32-S3-mikro-ohjaimen perustuvalla järjestelmällä.

## Mikrofonipiiri

## Signaalinkäsittelypiiri

<div style="float: left; width: 62%;">
  <img src="images/ESP32-DSP_chematic.jpg" style="width: 100%;" />
  <p>Kuva 1: ESP32-DSP piirikaavio</p>
</div>
<div style="float: right; width: 36%;">
  <img src="images/ESP32-DSP_pcb.jpg" style="width: 100%;" />
  <p>Kuva 2: ESP32-DSP piirikortti</p>
</div>


<div style="clear: both;">
  <img src="images/ESP32-DSP_pcb2.jpg" style="width: 60%;" />
  <p>Kuva 3: ESP32-DSP piirikortti kasattuna</p>
</div>



## TODO
* [x] DSP alustan suunnittelu
* [x] Datan keräys ADS8688-ADC:n avulla
* [ ] Beam forming algoritmi
* [x] Mikrofoonipiirin suunnittelu
* [ ] Äänen toisto käyttäjälle