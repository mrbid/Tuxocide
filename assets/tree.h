
#ifndef tree_H
#define tree_H

const GLfloat tree_vertices[] = {-0.027671,0.008991,0.22399,1e-06,0,0.280906,-0.021377,0.029424,0.218993,0.021379,0.029424,0.218993,0.027673,0.008991,0.22399,0.017103,-0.023539,0.22399,1e-06,-0.03637,0.218993,-0.034589,-0.011239,0.218993,1e-06,0.029096,0.22399,0.034591,-0.011239,0.218993,-0.017101,-0.023539,0.22399,-0.017101,0.02354,0.21388,0.017103,0.02354,0.21388,1e-06,-0.029096,0.21388,-0.027671,-0.008991,0.21388,0.027673,-0.008991,0.21388,0.043407,0.014104,0.19067,1e-06,-0.036511,0.197571,0.026827,-0.036923,0.19067,-0.043405,0.014104,0.19067,0.021462,0.029539,0.197571,1e-06,0.04564,0.19067,-0.034724,-0.011282,0.197571,-0.026825,-0.036923,0.19067,0.034725,-0.011282,0.197571,-0.02146,0.029539,0.197571,0.021462,-0.029538,0.184285,-0.034724,0.011283,0.184285,1e-06,0.036512,0.184285,-0.02146,-0.029538,0.184285,0.034725,0.011283,0.184285,-0.043492,0.014132,0.162581,-0.054365,-0.017664,0.152802,0.033601,0.046247,0.152802,0.026881,-0.036997,0.162581,0.054367,-0.017664,0.152802,1e-06,0.045731,0.162581,-0.033599,0.046247,0.152802,-0.026879,-0.036997,0.162581,1e-06,-0.057163,0.152802,0.043493,0.014132,0.162581,0.043493,-0.014131,0.144709,-0.043492,-0.014131,0.144709,-0.026879,0.036997,0.144709,1e-06,-0.045731,0.144709,0.026881,0.036997,0.144709,-0.054463,-0.017696,0.118368,-0.042075,-0.057912,0.105549,1e-06,0.071584,0.105549,0.054465,-0.017696,0.118368,0.068081,0.022121,0.105549,-0.03366,0.04633,0.118368,-0.068079,0.022121,0.105549,1e-06,-0.057267,0.118368,0.042077,-0.057912,0.105549,0.033661,0.04633,0.118368,1e-06,0.057267,0.095611,0.054465,0.017697,0.095611,-0.03366,-0.04633,0.095611,-0.054463,0.017697,0.095611,0.033661,-0.04633,0.095611,0.085211,-0.027686,0.044484,-0.042129,-0.057987,0.064216,1e-06,-0.089595,0.044484,-0.052662,0.072484,0.044484,0.068169,0.022149,0.064216,0.052664,0.072484,0.044484,-0.068167,0.022149,0.064216,-0.085209,-0.027686,0.044484,0.042131,-0.057987,0.064216,1e-06,0.071677,0.064216,1e-06,-0.051391,0.044484,-0.030206,0.041577,0.044484,0.030208,0.041577,0.044484,-0.048875,-0.015881,0.044484,0.048877,-0.015881,0.044484,-0.036866,-0.011979,-0.001553,1e-06,-0.038764,-0.001553,1e-06,-0.023184,0.06369,-0.022049,-0.007164,0.06369,-0.022784,0.031361,-0.001553,-0.013627,0.018757,0.06369,0.036868,-0.011979,-0.001553,0.022051,-0.007164,0.06369,0.022786,0.031361,-0.001553,0.013628,0.018757,0.06369};
const GLfloat tree_normals[] = {-0.896577,0.291316,0.333593,0,0,1,-0.570349,0.785019,-0.241759,0.570349,0.785019,-0.241759,0.896577,0.291316,0.333593,0.554115,-0.762674,0.333593,0,-0.970336,-0.241759,-0.922845,-0.299851,-0.241759,-0,0.942717,0.333593,0.922845,-0.299851,-0.241759,-0.554115,-0.762674,0.333593,-0.58669,0.807509,-0.061027,0.58669,0.807509,-0.061027,0,-0.998136,-0.061027,-0.949284,-0.308441,-0.061027,0.949284,-0.308441,-0.061027,0.915124,0.297342,-0.272278,0,-0.953916,0.300072,0.565578,-0.778451,-0.272278,-0.915124,0.297342,-0.272278,0.560698,0.771735,0.300072,-0,0.962219,-0.272278,-0.907228,-0.294776,0.300072,-0.565578,-0.778451,-0.272278,0.907228,-0.294776,0.300072,-0.560698,0.771735,0.300072,0.58632,-0.807,-0.070575,-0.948685,0.308246,-0.070575,-0,0.997506,-0.070575,-0.58632,-0.807,-0.070575,0.948685,0.308246,-0.070575,-0.911447,0.296147,0.28559,-0.910319,-0.29578,-0.28954,0.562608,0.774364,-0.28954,0.563305,-0.775323,0.28559,0.910319,-0.295781,-0.28954,-0,0.958352,0.28559,-0.562608,0.774364,-0.28954,-0.563305,-0.775323,0.28559,0,-0.957166,-0.28954,0.911447,0.296147,0.28559,0.949667,-0.308566,-0.054029,-0.949667,-0.308566,-0.054029,-0.586927,0.807835,-0.054029,0,-0.998539,-0.054029,0.586927,0.807835,-0.054029,-0.910528,-0.295849,0.288811,-0.561323,-0.772595,-0.29667,-0,0.95498,-0.296671,0.910528,-0.295848,0.288811,0.90824,0.295105,-0.29667,-0.562738,0.774542,0.288811,-0.90824,0.295105,-0.296671,-0,-0.957386,0.288811,0.561323,-0.772595,-0.29667,0.562737,0.774542,0.288811,-0,0.998484,-0.05504,0.949615,0.308548,-0.05504,-0.586894,-0.80779,-0.05504,-0.949615,0.308549,-0.05504,0.586894,-0.80779,-0.05504,0.802883,-0.260873,-0.536026,-0.575826,-0.792556,-0.200699,0,-0.844201,-0.536026,-0.496209,0.682973,-0.536027,0.931705,0.302729,-0.200699,0.496209,0.682973,-0.536026,-0.931705,0.30273,-0.200699,-0.802883,-0.260872,-0.536026,0.575826,-0.792556,-0.200699,-0,0.979653,-0.200699,-0,-0.129333,-0.991601,-0.07602,0.104633,-0.991601,0.07602,0.104633,-0.991601,-0.123003,-0.039966,-0.991601,0.123003,-0.039966,-0.991601,-0.925048,-0.300566,0.232265,0,-0.972652,0.232265,0,-0.972653,0.232265,-0.925048,-0.300566,0.232265,-0.571711,0.786892,0.232265,-0.571711,0.786892,0.232265,0.925048,-0.300566,0.232265,0.925048,-0.300566,0.232265,0.571711,0.786892,0.232265,0.571711,0.786893,0.232265};
const GLfloat tree_colors[] = {0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.267,0.412,0.573,0.38,0.243,0.129,0.38,0.243,0.129,0.38,0.243,0.129,0.38,0.243,0.129,0.38,0.243,0.129,0.38,0.243,0.129,0.38,0.243,0.129,0.38,0.243,0.129,0.38,0.243,0.129,0.38,0.243,0.129};
const GLubyte tree_indices[] = {0,1,2,3,1,4,5,1,6,7,1,0,8,1,3,9,1,5,10,1,7,2,1,8,4,1,9,6,1,10,2,11,0,4,12,3,6,13,5,0,14,7,3,12,8,5,15,9,7,14,10,8,11,2,9,15,4,10,13,6,12,4,16,13,17,18,14,0,19,12,20,21,15,5,18,14,22,23,11,8,21,15,24,16,13,10,23,11,25,19,17,26,18,19,27,22,20,28,21,18,26,24,22,29,23,21,28,25,24,30,16,23,29,17,25,27,19,16,30,20,27,31,32,28,20,33,26,34,35,29,22,32,28,36,37,30,24,35,29,38,39,27,25,37,30,40,33,26,17,39,34,41,35,32,42,38,36,43,37,35,41,40,38,44,39,37,43,31,40,45,33,39,44,34,31,42,32,33,45,36,42,46,47,43,36,48,41,49,50,44,38,47,43,51,52,45,40,50,44,53,54,42,31,52,45,55,48,41,34,54,48,56,51,49,57,50,47,58,53,51,59,52,50,57,55,53,60,54,52,59,46,55,56,48,54,60,49,46,58,47,57,49,61,58,62,63,59,51,64,57,65,66,60,53,63,59,67,68,56,55,66,60,69,61,58,46,68,56,70,64,62,71,63,64,72,67,65,73,66,63,71,69,67,74,68,66,73,70,69,75,61,68,74,62,70,72,64,61,75,65,62,74,71,69,71,75,65,75,73,72,70,73,67,72,74,12,16,20,13,18,5,14,19,22,12,21,8,15,18,24,14,23,10,11,21,25,15,16,4,13,23,17,11,19,0,27,32,22,28,33,36,26,35,24,29,32,38,28,37,25,30,35,40,29,39,17,27,37,31,30,33,20,26,39,34,42,47,38,43,48,51,41,50,40,44,47,53,43,52,31,45,50,55,44,54,34,42,52,46,45,48,36,41,54,49,57,61,65,58,63,53,59,64,67,57,66,55,60,63,69,59,68,46,56,66,70,60,61,49,58,68,62,56,64,51,73,74,72,71,74,75,75,74,73,76,77,78,76,78,79,80,76,79,80,79,81,77,82,83,77,83,78,82,84,85,82,85,83,84,80,81,84,81,85};
const GLsizeiptr tree_numind = 474;
const GLsizeiptr tree_numvert = 86;

#endif