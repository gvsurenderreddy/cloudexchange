DevStack Control/Compute Node 설치 문제 해결
===========================================

##### ERROR: error in setup command: Invalid environment marker: (python_version=='2.7') 
해결 방법 :
```
해당 *.py 파일을 열어서 (다음과 같이 바꾼다. 왠일인지 주석으로는 반영되지 않았음)
- ldappool>=1.0:python_version=='2.7' 
+ ldappool>=1.0
```

##### ERROR: create_network_postcommit failed.
```
2015-09-15 05:09:05.317 | ++ neutron net-create --tenant-id c9c0a95dd6564175939baf46fa012d72 private
2015-09-15 05:09:05.317 | ++ read data
2015-09-15 05:09:06.674 | create_network_postcommit failed.
2015-09-15 05:09:06.750 | + NET_ID=
2015-09-15 05:09:06.750 | + die_if_not_set 532 NET_ID 'Failure creating NET_ID for  c9c0a95dd6564175939baf46fa012d72'
2015-09-15 05:09:06.750 | + local exitcode=0
2015-09-15 05:09:06.758 | [Call Trace]
2015-09-15 05:09:06.758 | ./stack.sh:1293:create_neutron_initial_network
2015-09-15 05:09:06.758 | /home/stack/devstack/lib/neutron:532:die_if_not_set
2015-09-15 05:09:06.759 | /home/stack/devstack/functions-common:284:die
2015-09-15 05:09:06.789 | [ERROR] /home/stack/devstack/functions-common:532 Failure creating NET_ID for c9c0a95dd6564175939baf46fa012d72
2015-09-15 05:09:07.811 | Error on exit
2015-09-15 05:09:07.968 | df: '/run/user/1000/gvfs': Permission denied
2015-09-15 05:09:07.968 | df: '/media/djkim/VBOXADDITIONS_5.0.2_102096': Permission denied
```
해결 방법 :
```
'create_network_postcommit failed' 발생하는 이유는 ODL 설치가 잘못 되어서 인데. VTN manager 가 설지 되지 않은 문제.
```

##### ERROR: ImportError: No module named config
```
2015-09-18 07:28:57.344 | + mysql -uroot -psupersecret -h127.0.0.1 -e 'DROP DATABASE IF EXISTS keystone;'
2015-09-18 07:28:57.352 | + mysql -uroot -psupersecret -h127.0.0.1 -e 'CREATE DATABASE keystone CHARACTER SET utf8;'
2015-09-18 07:28:57.359 | + /opt/stack/keystone/bin/keystone-manage db_sync
2015-09-18 07:28:57.388 | Traceback (most recent call last):
2015-09-18 07:28:57.388 |   File "/opt/stack/keystone/bin/keystone-manage", line 30, in <module>
2015-09-18 07:28:57.388 |     from keystone import cli
2015-09-18 07:28:57.388 |   File "/opt/stack/keystone/keystone/cli.py", line 19, in <module>
2015-09-18 07:28:57.388 |     from oslo.config import cfg
2015-09-18 07:28:57.388 | ImportError: No module named config
2015-09-18 07:28:57.390 | + exit_trap
2015-09-18 07:28:57.390 | + local r=1
2015-09-18 07:28:57.391 | ++ jobs -p
2015-09-18 07:28:57.391 | + jobs=
2015-09-18 07:28:57.391 | + [[ -n '' ]]
2015-09-18 07:28:57.391 | + kill_spinner
2015-09-18 07:28:57.391 | + '[' '!' -z '' ']'
2015-09-18 07:28:57.391 | + [[ 1 -ne 0 ]]
2015-09-18 07:28:57.391 | + echo 'Error on exit'
2015-09-18 07:28:57.391 | Error on exit
2015-09-18 07:28:57.391 | + [[ -z /opt/stack/logs ]]
2015-09-18 07:28:57.391 | + /home/stack/devstack/tools/worlddump.py -d /opt/stack/logs
2015-09-18 07:28:57.454 | + exit 1
```
해결 방법 :
- sudo apt-get install python-oslo.config --->해결하기 위해 이렇게 하고 수많은 에러가 발생.
- 설치를 초기화 ./unstack.sh ,  ./clean.sh 등등 하고 재시도 (실패함)
- devstack 을 완전히 삭제 하고 다시 설치 --> 해결 않됨

##### INFO: devstack 삭제 방법 
```
sudo ./clean.sh
sudo rm -rf /home/stack/devstack /opt/stack
sudo rm /usr/local/bin/*
rm -rf /usr/local/lib/python2.7/distpackages/openstack* 
sudo rm -rf /etc/libvirt/qemu/inst*
sudo virsh list | grep inst | awk '{print $1}' | xargs -n1 virsh destroy
Delete user “stack” in linux --> 이과정에서 stack이 실행하고 있는 process를 kill로 죽여야 함
리붓 하고 다시 시도 함
```
이방법으로 완벽하게 삭제되지는 않는다. Control node 재 설치시 문제가 발생함.