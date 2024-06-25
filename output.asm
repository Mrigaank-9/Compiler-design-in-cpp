load 10 into eax
mov x, eax
load 20 into eax
mov y, eax
load x into eax
compare eax with y
if eax != 0 jump label
load x into eax
print eax
label:
load y into eax
print eax
label:
load x into eax
compare eax with 0
if eax == 0 jump end_label
load x into eax
substract eax with 1
mov x, eax
jump label
end_label:
