import re
import matplotlib.pyplot as plt
import numpy as np

# 设置中文字体（如果需要显示中文，取消对应系统的注释）
# plt.rcParams['font.sans-serif'] = ['SimHei']  # Windows系统
# plt.rcParams['font.sans-serif'] = ['PingFang SC']  # macOS系统
# plt.rcParams['axes.unicode_minus'] = False  # 解决负号显示问题

# 原始数据
data_text = """
set pos:0.075000, recived pos:0.000545, max time delay:9.927362, now delay:9.927362
set pos:0.150000, recived pos:0.000545, max time delay:19.927364, now delay:19.927364
set pos:0.225000, recived pos:0.000545, max time delay:29.927364, now delay:29.927364
set pos:0.300000, recived pos:0.004131, max time delay:39.449169, now delay:39.449169
set pos:0.375000, recived pos:0.020656, max time delay:47.245850, now delay:47.245850
set pos:0.450000, recived pos:0.055749, max time delay:52.566818, now delay:52.566818
set pos:0.525000, recived pos:0.110726, max time delay:55.236549, now delay:55.236549
set pos:0.600000, recived pos:0.167882, max time delay:57.615730, now delay:57.615730
set pos:0.675000, recived pos:0.228352, max time delay:59.553032, now delay:59.553032
set pos:0.750000, recived pos:0.292454, max time delay:61.006088, now delay:61.006088
set pos:0.825000, recived pos:0.358100, max time delay:62.253345, now delay:62.253345
set pos:0.900000, recived pos:0.429874, max time delay:62.683434, now delay:62.683434
set pos:0.975000, recived pos:0.496609, max time delay:63.785416, now delay:63.785416
set pos:1.050000, recived pos:0.564570, max time delay:64.723976, now delay:64.723976
set pos:1.125000, recived pos:0.636117, max time delay:65.184334, now delay:65.184334
set pos:1.200000, recived pos:0.709435, max time delay:65.408623, now delay:65.408623
set pos:1.275000, recived pos:0.788927, max time delay:65.408623, now delay:64.809700
set pos:1.350000, recived pos:0.859521, max time delay:65.408623, now delay:65.397179
set pos:1.425000, recived pos:0.929888, max time delay:66.014923, now delay:66.014923
set pos:1.500000, recived pos:1.003524, max time delay:66.196838, now delay:66.196838
set pos:1.575000, recived pos:1.078295, max time delay:66.227432, now delay:66.227432
set pos:1.650000, recived pos:1.152339, max time delay:66.354874, now delay:66.354874
set pos:1.725000, recived pos:1.230968, max time delay:66.354874, now delay:65.870964
set pos:1.800000, recived pos:1.303650, max time delay:66.354874, now delay:66.180008
set pos:1.875000, recived pos:1.377604, max time delay:66.354874, now delay:66.319542
set pos:1.950001, recived pos:1.452556, max time delay:66.354874, now delay:66.325928
set pos:2.025001, recived pos:1.526101, max time delay:66.519958, now delay:66.519958
set pos:2.100001, recived pos:1.597285, max time delay:67.028740, now delay:67.028740
set pos:2.175001, recived pos:1.669922, max time delay:67.343834, now delay:67.343834
set pos:2.250001, recived pos:1.752637, max time delay:67.343834, now delay:66.315140
set pos:2.325001, recived pos:1.828361, max time delay:67.343834, now delay:66.218628
set pos:2.400001, recived pos:1.901043, max time delay:67.343834, now delay:66.527664
set pos:2.475001, recived pos:1.972273, max time delay:67.343834, now delay:67.030396
set pos:2.550001, recived pos:2.044456, max time delay:67.406013, now delay:67.406013
set pos:2.625001, recived pos:2.120134, max time delay:67.406013, now delay:67.315544
set pos:2.700001, recived pos:2.204983, max time delay:67.406013, now delay:66.002365
set pos:2.775001, recived pos:2.278664, max time delay:67.406013, now delay:66.178223
set pos:2.850001, recived pos:2.349485, max time delay:67.406013, now delay:66.735428
set pos:2.925001, recived pos:2.421260, max time delay:67.406013, now delay:67.165535
set pos:3.000001, recived pos:2.497029, max time delay:67.406013, now delay:67.062943
set pos:3.075001, recived pos:2.573616, max time delay:67.406013, now delay:66.851418
set pos:3.150001, recived pos:2.648749, max time delay:67.406013, now delay:66.833588
set pos:3.225001, recived pos:2.727878, max time delay:67.406013, now delay:66.283096
set pos:3.300001, recived pos:2.799879, max time delay:67.406013, now delay:66.682938
set pos:3.375001, recived pos:2.873924, max time delay:67.406013, now delay:66.810387
set pos:3.450001, recived pos:2.949647, max time delay:67.406013, now delay:66.713875
set pos:3.525002, recived pos:3.023601, max time delay:67.406013, now delay:66.853424
set pos:3.600002, recived pos:3.095511, max time delay:67.406013, now delay:67.265350
set pos:3.675002, recived pos:3.176275, max time delay:67.406013, now delay:66.496941
set pos:3.750002, recived pos:3.251000, max time delay:67.406013, now delay:66.533592
set pos:3.825002, recived pos:3.326360, max time delay:67.406013, now delay:66.485497
set pos:3.900002, recived pos:3.399134, max time delay:67.406013, now delay:66.782410
set pos:3.975002, recived pos:3.470863, max time delay:67.406013, now delay:67.218559
set pos:4.050002, recived pos:3.543908, max time delay:67.479164, now delay:67.479164
set pos:4.125001, recived pos:3.628212, max time delay:67.479164, now delay:66.238564
set pos:4.200001, recived pos:3.705071, max time delay:67.479164, now delay:65.990700
set pos:4.275001, recived pos:3.778207, max time delay:67.479164, now delay:66.239159
set pos:4.350001, recived pos:3.848665, max time delay:67.479164, now delay:66.844780
set pos:4.425001, recived pos:3.920984, max time delay:67.479164, now delay:67.202179
set pos:4.500000, recived pos:3.997207, max time delay:67.479164, now delay:67.039078
set pos:4.575000, recived pos:4.073794, max time delay:67.479164, now delay:66.827515
set pos:4.650000, recived pos:4.155465, max time delay:67.479164, now delay:65.937988
set pos:4.725000, recived pos:4.225696, max time delay:67.479164, now delay:66.573837
set pos:4.800000, recived pos:4.297833, max time delay:67.479164, now delay:66.955498
set pos:4.875000, recived pos:4.372740, max time delay:67.479164, now delay:66.967896
set pos:4.949999, recived pos:4.448964, max time delay:67.479164, now delay:66.804756
set pos:5.024999, recived pos:4.522645, max time delay:67.479164, now delay:66.980614
set pos:5.099999, recived pos:4.601819, max time delay:67.479164, now delay:66.424049
set pos:5.174999, recived pos:4.674274, max time delay:67.479164, now delay:66.763298
set pos:5.249999, recived pos:4.748954, max time delay:67.479164, now delay:66.805962
set pos:5.324998, recived pos:4.832078, max time delay:67.479164, now delay:65.722778
set pos:5.399998, recived pos:4.904942, max time delay:67.479164, now delay:66.007545
set pos:5.474998, recived pos:4.975762, max time delay:67.479164, now delay:66.564743
set pos:5.549998, recived pos:5.047673, max time delay:67.479164, now delay:66.976608
set pos:5.624998, recived pos:5.123533, max time delay:67.479164, now delay:66.861908
set pos:5.699997, recived pos:5.199530, max time delay:67.479164, now delay:66.729034
set pos:5.774997, recived pos:5.280746, max time delay:67.479164, now delay:65.900101
set pos:5.849997, recived pos:5.352657, max time delay:67.479164, now delay:66.311958
set pos:5.924997, recived pos:5.425703, max time delay:67.479164, now delay:66.572563
set pos:5.999997, recived pos:5.500791, max time delay:67.479164, now delay:66.560738
set pos:6.074996, recived pos:5.575970, max time delay:67.479164, now delay:66.536835
set pos:6.149996, recived pos:5.648970, max time delay:67.479164, now delay:66.803482
set pos:6.224996, recived pos:5.720336, max time delay:67.479164, now delay:67.288010
set pos:6.299996, recived pos:5.801961, max time delay:67.479164, now delay:66.404594
set pos:6.374996, recived pos:5.877005, max time delay:67.479164, now delay:66.398804
set pos:6.449996, recived pos:5.951820, max time delay:67.479164, now delay:66.423347
set pos:6.524995, recived pos:6.023550, max time delay:67.479164, now delay:66.859428
set pos:6.599995, recived pos:6.095006, max time delay:67.479164, now delay:67.331879
set pos:6.674995, recived pos:6.176541, max time delay:67.479164, now delay:66.460541
set pos:6.749995, recived pos:6.253627, max time delay:67.479164, now delay:66.182388
set pos:6.824995, recived pos:6.328534, max time delay:67.479164, now delay:66.194786
set pos:6.899994, recived pos:6.399990, max time delay:67.479164, now delay:66.667236
set pos:6.974994, recived pos:6.471084, max time delay:67.479164, now delay:67.188065
set pos:7.049994, recived pos:6.552392, max time delay:67.479164, now delay:66.346992
set pos:7.124994, recived pos:6.629659, max time delay:67.479164, now delay:66.044678
set pos:7.199994, recived pos:6.704384, max time delay:67.479164, now delay:66.081299
set pos:7.274993, recived pos:6.776022, max time delay:67.479164, now delay:66.529526
set pos:7.349993, recived pos:6.846616, max time delay:67.479164, now delay:67.116920
set pos:7.424993, recived pos:6.920660, max time delay:67.479164, now delay:67.244331
set pos:7.499993, recived pos:7.005146, max time delay:67.479164, now delay:65.979568
set pos:7.574993, recived pos:7.081142, max time delay:67.479164, now delay:65.846695
set pos:7.649992, recived pos:7.152599, max time delay:67.479164, now delay:66.319145
set pos:7.724992, recived pos:7.222739, max time delay:67.479164, now delay:66.967133
set pos:7.799992, recived pos:7.296329, max time delay:67.479164, now delay:67.155067
set pos:7.874992, recived pos:7.372325, max time delay:67.479164, now delay:67.022194
set pos:7.949992, recived pos:7.455904, max time delay:67.479164, now delay:65.878418
set pos:8.024992, recived pos:7.526997, max time delay:67.479164, now delay:66.399376
set pos:8.099992, recived pos:7.597682, max time delay:67.479164, now delay:66.974701
set pos:8.174992, recived pos:7.671000, max time delay:67.479164, now delay:67.198936
set pos:8.249991, recived pos:7.755167, max time delay:67.479164, now delay:65.976517
set pos:8.324991, recived pos:7.830709, max time delay:67.479164, now delay:65.904228
set pos:8.399991, recived pos:7.902302, max time delay:67.479164, now delay:66.358498
set pos:8.474991, recived pos:7.973078, max time delay:67.479164, now delay:66.921738
set pos:8.549991, recived pos:8.054431, max time delay:67.479164, now delay:66.074623
set pos:8.624990, recived pos:8.129792, max time delay:67.479164, now delay:66.026428
set pos:8.699990, recived pos:8.203292, max time delay:67.479164, now delay:66.226448
set pos:8.774990, recived pos:8.274930, max time delay:67.479164, now delay:66.674675
set pos:8.849990, recived pos:8.346250, max time delay:67.479164, now delay:67.165367
set pos:8.924990, recived pos:8.421157, max time delay:67.479164, now delay:67.177704
set pos:8.999990, recived pos:8.505778, max time delay:67.479164, now delay:65.894821
set pos:9.074989, recived pos:8.579959, max time delay:67.479164, now delay:66.004051
set pos:9.149989, recived pos:8.650372, max time delay:67.479164, now delay:66.615669
set pos:9.224989, recived pos:8.721283, max time delay:67.479164, now delay:67.160789
set pos:9.299989, recived pos:8.795781, max time delay:67.479164, now delay:67.227676
set pos:9.374989, recived pos:8.880040, max time delay:67.479164, now delay:65.993111
set pos:9.449988, recived pos:8.955583, max time delay:67.479164, now delay:65.920761
set pos:9.524988, recived pos:9.026540, max time delay:67.479164, now delay:66.459778
set pos:9.599988, recived pos:9.097588, max time delay:67.479164, now delay:66.986717
set pos:9.674988, recived pos:9.171495, max time delay:67.479164, now delay:67.132309
set pos:9.749988, recived pos:9.248173, max time delay:67.479164, now delay:66.908638
set pos:9.824987, recived pos:9.330661, max time delay:67.479164, now delay:65.910210
set pos:9.899987, recived pos:9.403253, max time delay:67.479164, now delay:66.231277
set pos:9.974987, recived pos:9.474573, max time delay:67.479164, now delay:66.721848
set pos:10.049987, recived pos:9.548254, max time delay:67.479164, now delay:66.897705
set pos:10.124987, recived pos:9.624250, max time delay:67.479164, now delay:66.764824
set pos:10.199986, recived pos:9.699066, max time delay:67.479164, now delay:66.789368
set pos:10.274986, recived pos:9.778831, max time delay:67.479164, now delay:66.154091
set pos:10.349986, recived pos:9.851332, max time delay:67.479164, now delay:66.487244
set pos:10.424986, recived pos:9.926011, max time delay:67.479164, now delay:66.529968
set pos:10.499986, recived pos:10.001009, max time delay:67.479164, now delay:66.530220
set pos:10.574986, recived pos:10.075280, max time delay:67.479164, now delay:66.627373
set pos:10.649985, recived pos:10.146873, max time delay:67.479164, now delay:67.081703
set pos:10.724985, recived pos:10.219419, max time delay:67.479164, now delay:67.408875
set pos:10.799985, recived pos:10.302588, max time delay:67.479164, now delay:66.319527
set pos:10.874985, recived pos:10.378222, max time delay:67.479164, now delay:66.235092
set pos:10.949985, recived pos:10.451493, max time delay:67.479164, now delay:66.465500
set pos:11.024984, recived pos:10.522269, max time delay:67.479164, now delay:67.028679
set pos:11.099984, recived pos:10.594769, max time delay:67.479164, now delay:67.361954
set pos:11.174984, recived pos:10.669994, max time delay:67.479164, now delay:67.331947
set pos:11.249984, recived pos:10.754980, max time delay:67.479164, now delay:66.000488
set pos:11.324984, recived pos:10.829796, max time delay:67.479164, now delay:66.025032
set pos:11.399983, recived pos:10.901025, max time delay:67.479164, now delay:66.527809
set pos:11.474983, recived pos:10.972118, max time delay:67.479164, now delay:67.048637
set pos:11.549983, recived pos:11.046889, max time delay:67.479164, now delay:67.079155
set pos:11.624983, recived pos:11.131193, max time delay:67.479164, now delay:65.838615
set pos:11.699983, recived pos:11.204511, max time delay:67.479164, now delay:66.062920
set pos:11.774982, recived pos:11.274923, max time delay:67.479164, now delay:66.674545
set pos:11.849982, recived pos:11.346471, max time delay:67.479164, now delay:67.134850
set pos:11.924982, recived pos:11.422150, max time delay:67.479164, now delay:67.044319
set pos:11.999982, recived pos:11.506499, max time delay:67.479164, now delay:65.797676
set pos:12.074982, recived pos:11.579636, max time delay:67.479164, now delay:66.046135
set pos:12.149981, recived pos:11.649457, max time delay:67.479164, now delay:66.736595
set pos:12.224981, recived pos:11.728859, max time delay:67.479164, now delay:66.149643
set pos:12.299981, recived pos:11.804265, max time delay:67.479164, now delay:66.095474
set pos:12.374981, recived pos:11.879762, max time delay:67.479164, now delay:66.029228
set pos:12.449981, recived pos:11.952127, max time delay:67.479164, now delay:66.380562
set pos:12.524981, recived pos:12.022675, max time delay:67.479164, now delay:66.974129
set pos:12.599980, recived pos:12.095221, max time delay:67.479164, now delay:67.301300
set pos:12.674980, recived pos:12.170899, max time delay:67.479164, now delay:67.210762
set pos:12.749980, recived pos:12.255612, max time delay:67.479164, now delay:65.915672
set pos:12.824980, recived pos:12.329520, max time delay:67.479164, now delay:66.061272
set pos:12.899980, recived pos:12.400432, max time delay:67.479164, now delay:66.606392
set pos:12.974979, recived pos:12.471934, max time delay:67.479164, now delay:67.072670
set pos:13.049979, recived pos:12.547204, max time delay:67.479164, now delay:67.036690
set pos:13.124979, recived pos:12.623518, max time delay:67.479164, now delay:66.861465
set pos:13.199979, recived pos:12.705189, max time delay:67.479164, now delay:65.972008
set pos:13.274979, recived pos:12.777100, max time delay:67.479164, now delay:66.383865
set pos:13.349978, recived pos:12.849328, max time delay:67.479164, now delay:66.753380
set pos:13.424978, recived pos:12.924598, max time delay:67.479164, now delay:66.717400
set pos:13.499978, recived pos:13.000186, max time delay:67.479164, now delay:66.638939
set pos:13.574978, recived pos:13.074048, max time delay:67.479164, now delay:66.790642
set pos:13.649978, recived pos:13.152678, max time delay:67.479164, now delay:66.306679
set pos:13.724977, recived pos:13.226768, max time delay:67.479164, now delay:66.427986
set pos:13.799977, recived pos:13.301674, max time delay:67.479164, now delay:66.440453
set pos:13.874977, recived pos:13.376944, max time delay:67.479164, now delay:66.404465
set pos:13.949977, recived pos:13.449445, max time delay:67.479164, now delay:66.737617
set pos:14.024977, recived pos:13.520629, max time delay:67.479164, now delay:67.246368
set pos:14.099977, recived pos:13.601619, max time delay:67.479164, now delay:66.447701
set pos:14.174976, recived pos:13.678251, max time delay:67.479164, now delay:66.230003
set pos:14.249976, recived pos:13.753839, max time delay:67.479164, now delay:66.151680
set pos:14.324976, recived pos:13.825659, max time delay:67.479164, now delay:66.575615
set pos:14.399976, recived pos:13.903833, max time delay:67.479164, now delay:66.152313
set pos:14.474976, recived pos:13.977106, max time delay:67.479164, now delay:66.382591
set pos:14.549975, recived pos:14.052739, max time delay:67.479164, now delay:66.298164
set pos:14.624975, recived pos:14.127419, max time delay:67.479164, now delay:66.340759
set pos:14.699975, recived pos:14.200102, max time delay:67.479164, now delay:66.649750
set pos:14.774975, recived pos:14.271194, max time delay:67.479164, now delay:67.170708
set pos:14.849975, recived pos:14.352457, max time delay:67.479164, now delay:66.335670
set pos:14.924974, recived pos:14.428045, max time delay:67.479164, now delay:66.257217
set pos:14.999974, recived pos:14.502815, max time delay:67.479164, now delay:66.287865
set pos:15.074974, recived pos:14.575089, max time delay:67.479164, now delay:66.651276
set pos:14.999974, recived pos:14.645910, max time delay:67.479164, now delay:47.208527
set pos:14.924974, recived pos:14.719818, max time delay:67.479164, now delay:27.354176
set pos:14.849975, recived pos:14.803941, max time delay:67.479164, now delay:6.137848
set pos:14.774975, recived pos:14.869495, max time delay:67.479164, now delay:12.602741
set pos:14.699975, recived pos:14.897507, max time delay:67.479164, now delay:26.337559
set pos:14.624975, recived pos:14.893284, max time delay:67.479164, now delay:35.774483
set pos:14.549975, recived pos:14.874580, max time delay:67.479164, now delay:43.280663
set pos:14.474976, recived pos:14.839850, max time delay:67.479164, now delay:48.649979
set pos:14.399976, recived pos:14.797132, max time delay:67.479164, now delay:52.954102
set pos:14.324976, recived pos:14.746467, max time delay:67.479164, now delay:56.198753
set pos:14.249976, recived pos:14.688902, max time delay:67.479164, now delay:58.523430
set pos:14.174976, recived pos:14.627025, max time delay:67.479164, now delay:60.273102
set pos:14.099977, recived pos:14.561833, max time delay:67.479164, now delay:61.580910
set pos:14.024977, recived pos:14.488697, max time delay:67.479164, now delay:61.829372
set pos:13.949977, recived pos:14.424504, max time delay:67.479164, now delay:63.270309
set pos:13.874977, recived pos:14.358450, max time delay:67.479164, now delay:64.463036
set pos:13.799977, recived pos:14.288991, max time delay:67.479164, now delay:65.201820
set pos:13.724977, recived pos:14.208182, max time delay:67.479164, now delay:64.427307
set pos:13.649978, recived pos:14.134319, max time delay:67.479164, now delay:64.578880
set pos:13.574978, recived pos:14.064406, max time delay:67.479164, now delay:65.257133
set pos:13.499978, recived pos:13.995584, max time delay:67.479164, now delay:66.080727
set pos:13.424978, recived pos:13.924354, max time delay:67.479164, now delay:66.583374
set pos:13.349978, recived pos:13.849038, max time delay:67.479164, now delay:66.541283
set pos:13.274979, recived pos:13.766050, max time delay:67.479164, now delay:65.476219
set pos:13.199979, recived pos:13.694367, max time delay:67.479164, now delay:65.918472
set pos:13.124979, recived pos:13.624953, max time delay:67.479164, now delay:66.663231
set pos:13.049979, recived pos:13.552725, max time delay:67.479164, now delay:67.032745
set pos:12.974979, recived pos:13.477138, max time delay:67.479164, now delay:66.954414
set pos:12.899980, recived pos:13.400824, max time delay:67.479164, now delay:66.779198
set pos:12.824980, recived pos:13.319742, max time delay:67.479164, now delay:65.968315
set pos:12.749980, recived pos:13.247923, max time delay:67.479164, now delay:66.392387
set pos:12.674980, recived pos:13.174877, max time delay:67.479164, now delay:66.652931
set pos:12.599980, recived pos:13.100379, max time delay:67.479164, now delay:66.719810
set pos:12.524981, recived pos:13.024700, max time delay:67.479164, now delay:66.629280
set pos:12.449981, recived pos:12.951610, max time delay:67.479164, now delay:66.883842
set pos:12.374981, recived pos:12.872027, max time delay:67.479164, now delay:66.272858
set pos:12.299981, recived pos:12.798074, max time delay:67.479164, now delay:66.412346
set pos:12.224981, recived pos:12.722576, max time delay:67.479164, now delay:66.345970
set pos:12.149981, recived pos:12.647624, max time delay:67.479164, now delay:66.352333
set pos:12.074982, recived pos:12.575397, max time delay:67.479164, now delay:66.721977
set pos:11.999982, recived pos:12.503713, max time delay:67.479164, now delay:67.164101
set pos:11.924982, recived pos:12.422450, max time delay:67.479164, now delay:66.329063
set pos:11.849982, recived pos:12.345546, max time delay:67.479164, now delay:66.075127
set pos:11.774982, recived pos:12.270730, max time delay:67.479164, now delay:66.099670
set pos:11.699983, recived pos:12.198865, max time delay:67.479164, now delay:66.517632
set pos:11.624983, recived pos:12.128498, max time delay:67.479164, now delay:67.135361
set pos:11.549983, recived pos:12.054998, max time delay:67.479164, now delay:67.335381
set pos:11.474983, recived pos:11.978548, max time delay:67.479164, now delay:67.141975
set pos:11.399983, recived pos:11.894471, max time delay:67.479164, now delay:65.931694
set pos:11.324984, recived pos:11.821289, max time delay:67.479164, now delay:66.174057
set pos:11.249984, recived pos:11.750559, max time delay:67.479164, now delay:66.743340
set pos:11.174984, recived pos:11.677513, max time delay:67.479164, now delay:67.003883
set pos:11.099984, recived pos:11.602107, max time delay:67.479164, now delay:66.949715
set pos:11.024984, recived pos:11.518394, max time delay:67.479164, now delay:65.787880
set pos:10.949985, recived pos:11.446892, max time delay:67.479164, now delay:66.254295
set pos:10.874985, recived pos:11.376070, max time delay:67.479164, now delay:66.811363
set pos:10.799985, recived pos:11.302934, max time delay:67.479164, now delay:67.059830
set pos:10.724985, recived pos:11.226575, max time delay:67.479164, now delay:66.878632
set pos:10.649985, recived pos:11.150578, max time delay:67.479164, now delay:66.745750
set pos:10.574986, recived pos:11.070224, max time delay:67.479164, now delay:66.031769
set pos:10.499986, recived pos:10.997995, max time delay:67.479164, now delay:66.401283
set pos:10.424986, recived pos:10.924905, max time delay:67.479164, now delay:66.655853
set pos:10.349986, recived pos:10.849499, max time delay:67.479164, now delay:66.601685
set pos:10.274986, recived pos:10.774909, max time delay:67.479164, now delay:66.656364
set pos:10.199986, recived pos:10.702318, max time delay:67.479164, now delay:66.977562
set pos:10.124987, recived pos:10.623325, max time delay:67.479164, now delay:66.445152
set pos:10.049987, recived pos:10.548146, max time delay:67.479164, now delay:66.421249
set pos:9.974987, recived pos:10.472740, max time delay:67.479164, now delay:66.367081
set pos:9.899987, recived pos:10.398333, max time delay:67.479164, now delay:66.446045
set pos:9.824987, recived pos:10.326921, max time delay:67.479164, now delay:66.924538
set pos:9.749988, recived pos:10.255147, max time delay:67.479164, now delay:67.354576
set pos:9.674988, recived pos:10.172613, max time delay:67.479164, now delay:66.350044
set pos:9.599988, recived pos:10.095528, max time delay:67.479164, now delay:66.071953
set pos:9.524988, recived pos:10.020802, max time delay:67.479164, now delay:66.108574
set pos:9.449988, recived pos:9.950027, max time delay:67.479164, now delay:66.671745
set pos:9.374989, recived pos:9.878344, max time delay:67.479164, now delay:67.113998
set pos:9.299989, recived pos:9.804117, max time delay:67.479164, now delay:67.217125
set pos:9.224989, recived pos:9.719223, max time delay:67.479164, now delay:65.897873
set pos:9.149989, recived pos:9.645042, max time delay:67.479164, now delay:66.007103
set pos:9.074989, recived pos:9.574222, max time delay:67.479164, now delay:66.564301
set pos:8.999990, recived pos:9.503174, max time delay:67.479164, now delay:67.091240
set pos:8.924990, recived pos:9.428903, max time delay:67.479164, now delay:67.188385
set pos:8.849990, recived pos:9.344326, max time delay:67.479164, now delay:65.911476
set pos:8.774990, recived pos:9.269011, max time delay:67.479164, now delay:65.869392
set pos:8.699990, recived pos:9.197963, max time delay:67.479164, now delay:66.396324
set pos:8.624990, recived pos:9.127550, max time delay:67.479164, now delay:67.007950
set pos:8.549991, recived pos:9.053596, max time delay:67.479164, now delay:67.147438
set pos:8.474991, recived pos:8.977419, max time delay:67.479164, now delay:66.990402
set pos:8.399991, recived pos:8.894204, max time delay:67.479164, now delay:65.895073
set pos:8.324991, recived pos:8.822158, max time delay:67.479164, now delay:66.288879
set pos:8.249991, recived pos:8.750610, max time delay:67.479164, now delay:66.749184
set pos:8.174992, recived pos:8.676884, max time delay:67.479164, now delay:66.918938
set pos:8.099992, recived pos:8.601024, max time delay:67.479164, now delay:66.804245
set pos:8.024992, recived pos:8.518581, max time delay:67.479164, now delay:65.811913
set pos:7.949992, recived pos:8.446444, max time delay:67.479164, now delay:66.193512
set pos:7.874992, recived pos:8.374623, max time delay:67.479164, now delay:66.617455
set pos:7.799993, recived pos:8.301079, max time delay:67.479164, now delay:66.811493
set pos:7.724993, recived pos:8.224811, max time delay:67.479164, now delay:66.642372
set pos:7.649993, recived pos:8.150403, max time delay:67.479164, now delay:66.721336
set pos:7.574993, recived pos:8.078084, max time delay:67.479164, now delay:67.078773
set pos:7.499993, recived pos:7.998047, max time delay:67.479164, now delay:66.407135
set pos:7.424994, recived pos:7.923549, max time delay:67.479164, now delay:66.474022
set pos:7.349994, recived pos:7.848460, max time delay:67.479164, now delay:66.462196
set pos:7.274994, recived pos:7.774870, max time delay:67.479164, now delay:66.650131
set pos:7.199994, recived pos:7.703323, max time delay:67.479164, now delay:67.110504
set pos:7.124994, recived pos:7.631004, max time delay:67.479164, now delay:67.467941
set pos:7.049994, recived pos:7.547471, max time delay:67.479164, now delay:66.330208
set pos:6.974995, recived pos:7.470613, max time delay:67.479164, now delay:66.082375
set pos:6.899995, recived pos:7.396841, max time delay:67.479164, now delay:66.246094
set pos:6.824995, recived pos:7.326655, max time delay:67.479164, now delay:66.887978
set pos:6.749995, recived pos:7.254472, max time delay:67.479164, now delay:67.263596
set pos:6.674995, recived pos:7.179157, max time delay:67.479164, now delay:67.221512
set pos:6.599996, recived pos:7.094172, max time delay:67.479164, now delay:65.890114
set pos:6.524996, recived pos:7.019583, max time delay:67.479164, now delay:65.944923
set pos:6.449996, recived pos:6.948762, max time delay:67.479164, now delay:66.502121
set pos:6.374996, recived pos:6.877078, max time delay:67.479164, now delay:66.944244
set pos:6.299996, recived pos:6.802489, max time delay:67.479164, now delay:66.999046
set pos:6.224997, recived pos:6.725721, max time delay:67.479164, now delay:66.763298
set pos:6.149997, recived pos:6.643959, max time delay:67.479164, now delay:65.861633
set pos:6.074997, recived pos:6.573138, max time delay:67.479164, now delay:66.418831
set pos:5.999997, recived pos:6.501954, max time delay:67.479164, now delay:66.927589
set pos:5.924997, recived pos:6.427093, max time delay:67.479164, now delay:66.946022
set pos:5.849998, recived pos:6.350915, max time delay:67.479164, now delay:66.788986
set pos:5.774998, recived pos:6.269244, max time delay:67.479164, now delay:65.899460
set pos:5.699998, recived pos:6.197515, max time delay:67.479164, now delay:66.335609
set pos:5.624998, recived pos:6.125559, max time delay:67.479164, now delay:66.741432
set pos:5.549998, recived pos:6.051015, max time delay:67.479164, now delay:66.802277
set pos:5.474998, recived pos:5.975564, max time delay:67.479164, now delay:66.742004
set pos:5.399999, recived pos:5.901519, max time delay:67.479164, now delay:66.869415
set pos:5.324999, recived pos:5.822709, max time delay:67.479164, now delay:66.361298
set pos:5.249999, recived pos:5.748664, max time delay:67.479164, now delay:66.488708
set pos:5.174999, recived pos:5.673803, max time delay:67.479164, now delay:66.507141
set pos:5.099999, recived pos:5.598306, max time delay:67.479164, now delay:66.440895
set pos:5.025000, recived pos:5.525351, max time delay:67.479164, now delay:66.713516
set pos:4.950000, recived pos:5.453668, max time delay:67.479164, now delay:67.155708
set pos:4.875000, recived pos:5.372678, max time delay:67.479164, now delay:66.357033
set pos:4.800000, recived pos:5.297635, max time delay:67.479164, now delay:66.351250
set pos:4.725000, recived pos:5.222456, max time delay:67.479164, now delay:66.327347
set pos:4.650001, recived pos:5.150000, max time delay:67.479164, now delay:66.666595
set pos:4.575001, recived pos:5.078407, max time delay:67.479164, now delay:67.120865
set pos:4.500001, recived pos:4.997780, max time delay:67.479164, now delay:66.370575
set pos:4.425001, recived pos:4.922374, max time delay:67.479164, now delay:66.316406
set pos:4.350001, recived pos:4.847831, max time delay:67.479164, now delay:66.377251
set pos:4.275002, recived pos:4.775194, max time delay:67.479164, now delay:66.692284
set pos:4.200002, recived pos:4.704101, max time delay:67.479164, now delay:67.213181
set pos:4.125002, recived pos:4.623065, max time delay:67.479164, now delay:66.408409
set pos:4.050002, recived pos:4.546569, max time delay:67.479164, now delay:66.208961
set pos:3.975002, recived pos:4.471209, max time delay:67.479164, now delay:66.160866
set pos:3.900002, recived pos:4.399071, max time delay:67.479164, now delay:66.542557
set pos:3.825002, recived pos:4.328523, max time delay:67.479164, now delay:67.136093
set pos:3.750002, recived pos:4.247305, max time delay:67.479164, now delay:66.307129
set pos:3.675002, recived pos:4.172399, max time delay:67.479164, now delay:66.319557
set pos:3.600002, recived pos:4.097310, max time delay:67.479164, now delay:66.307762
set pos:3.525002, recived pos:4.025127, max time delay:67.479164, now delay:66.683418
set pos:3.450002, recived pos:3.953489, max time delay:67.479164, now delay:67.131676
set pos:3.375002, recived pos:3.872272, max time delay:67.479164, now delay:66.302711
set pos:3.300002, recived pos:3.795459, max time delay:67.479164, now delay:66.060921
set pos:3.225002, recived pos:3.720279, max time delay:67.479164, now delay:66.037048
set pos:3.150002, recived pos:3.648777, max time delay:67.479164, now delay:66.503456
set pos:3.075001, recived pos:3.577956, max time delay:67.479164, now delay:67.060654
set pos:3.000001, recived pos:3.504730, max time delay:67.479164, now delay:67.297073
set pos:2.925001, recived pos:3.420471, max time delay:67.479164, now delay:66.062576
set pos:2.850001, recived pos:3.344247, max time delay:67.479164, now delay:65.899460
set pos:2.775001, recived pos:3.272019, max time delay:67.479164, now delay:66.269043
set pos:2.700001, recived pos:3.202061, max time delay:67.479164, now delay:66.941254
set pos:2.625001, recived pos:3.129061, max time delay:67.479164, now delay:67.207901
set pos:2.550001, recived pos:3.053064, max time delay:67.479164, now delay:67.075089
set pos:2.475001, recived pos:2.976705, max time delay:67.479164, now delay:66.893829
set pos:2.400001, recived pos:2.895896, max time delay:67.479164, now delay:66.119354
set pos:2.325001, recived pos:2.825166, max time delay:67.479164, now delay:66.688660
set pos:2.250001, recived pos:2.752257, max time delay:67.479164, now delay:66.967453
set pos:2.175001, recived pos:2.676805, max time delay:67.479164, now delay:66.907242
set pos:2.100001, recived pos:2.600718, max time delay:67.479164, now delay:66.762283
set pos:2.025001, recived pos:2.520545, max time delay:67.479164, now delay:66.072586
set pos:1.950001, recived pos:2.449270, max time delay:67.479164, now delay:66.569260
set pos:1.875001, recived pos:2.376543, max time delay:67.479164, now delay:66.872246
set pos:1.800001, recived pos:2.300728, max time delay:67.479164, now delay:66.763618
set pos:1.725001, recived pos:2.217695, max time delay:67.479164, now delay:65.692551
set pos:1.650001, recived pos:2.144876, max time delay:67.479164, now delay:65.983421
set pos:1.575001, recived pos:2.074055, max time delay:67.479164, now delay:66.540649
set pos:1.500000, recived pos:2.001418, max time delay:67.479164, now delay:66.855713
set pos:1.425000, recived pos:1.925604, max time delay:67.479164, now delay:66.747108
set pos:1.350000, recived pos:1.850425, max time delay:67.479164, now delay:66.723221
set pos:1.275000, recived pos:1.777515, max time delay:67.479164, now delay:67.001991
set pos:1.200000, recived pos:1.698432, max time delay:67.479164, now delay:66.457535
set pos:1.125000, recived pos:1.624705, max time delay:67.479164, now delay:66.627357
set pos:1.050000, recived pos:1.549708, max time delay:67.479164, now delay:66.627686
set pos:0.975000, recived pos:1.474801, max time delay:67.479164, now delay:66.640106
set pos:0.900000, recived pos:1.402573, max time delay:67.479164, now delay:67.009666
set pos:0.825000, recived pos:1.323081, max time delay:67.479164, now delay:66.410736
set pos:0.750000, recived pos:1.247902, max time delay:67.479164, now delay:66.386841
set pos:0.675000, recived pos:1.172178, max time delay:67.479164, now delay:66.290321
set pos:0.600000, recived pos:1.097816, max time delay:67.479164, now delay:66.375381
set pos:0.525000, recived pos:1.026677, max time delay:67.479164, now delay:66.890213
set pos:0.450000, recived pos:0.954585, max time delay:67.479164, now delay:67.277924
set pos:0.375000, recived pos:0.872460, max time delay:67.479164, now delay:66.327919
set pos:0.300000, recived pos:0.795011, max time delay:67.479164, now delay:66.001373
set pos:0.225000, recived pos:0.720739, max time delay:67.479164, now delay:66.098541
set pos:0.150000, recived pos:0.649828, max time delay:67.479164, now delay:66.643654
set pos:0.075000, recived pos:0.578689, max time delay:67.479164, now delay:67.158478
set pos:0.000000, recived pos:0.504327, max time delay:67.479164, now delay:67.243591
set pos:0.000000, recived pos:0.419614, max time delay:67.479164, now delay:55.948555
"""

# 解析数据
lines = [line.strip() for line in data_text.strip().split('\n') if line.strip()]
max_delay = []
now_delay = []

# 正则表达式匹配数值
pattern = re.compile(r'max time delay:([\d.]+), now delay:([\d.]+)')

for line in lines:
    match = pattern.search(line)
    if match:
        max_val = float(match.group(1))
        now_val = float(match.group(2))
        max_delay.append(max_val)
        now_delay.append(now_val)

# 创建X轴数据（行号）
x = np.arange(1, len(max_delay) + 1)

# 设置图表大小
plt.figure(figsize=(12, 6))

# 创建双Y轴，一个线性（max delay），一个对数（now delay）
ax1 = plt.gca()  # 主坐标轴（左侧，线性）
ax2 = ax1.twinx()  # 次坐标轴（右侧，对数）

# 绘制max time delay（线性Y轴）
line1 = ax1.plot(x, max_delay, label='Max Time Delay (Linear)', 
                 color='red', linewidth=2, marker='o', markersize=3)

# 绘制now delay（对数Y轴）
line2 = ax2.plot(x, now_delay, label='Now Delay (Log)', 
                 color='blue', linewidth=2, marker='s', markersize=3)

# 设置主坐标轴（线性）
ax1.set_xlabel('Data Row Number', fontsize=12)
ax1.set_ylabel('Max Time Delay (Linear Scale)', fontsize=12, color='red')
ax1.tick_params(axis='y', labelcolor='red')
ax1.grid(True, alpha=0.3)

# 设置次坐标轴（对数）
ax2.set_ylabel('Now Delay (Log Scale)', fontsize=12, color='blue')
ax2.set_yscale('log')  # 关键：设置对数尺度
ax2.tick_params(axis='y', labelcolor='blue')

# 合并图例
lines = line1 + line2
labels = [l.get_label() for l in lines]
ax1.legend(lines, labels, loc='upper left', fontsize=10)

# 设置标题和X轴范围
plt.title('Time Delay Comparison (Now Delay in Log Scale)', fontsize=14, fontweight='bold')
ax1.set_xlim(0, len(x) + 1)

# 保存图片（dpi设置清晰度，bbox_inches避免标签被裁剪）
plt.savefig('time_delay_log_chart.jpg', dpi=300, bbox_inches='tight')
plt.close()

print("对数尺度图表已生成并保存为 time_delay_log_chart.jpg")
