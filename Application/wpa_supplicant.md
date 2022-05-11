# wpa_supplicant 连接校园网

### 1.断开网络

```bash
sudo service networking stop
```

### 2.编辑 wpa_supplicant.conf 文件

```bash
sudo nano /etc/wpa_supplicant/wpa_supplicant.conf
```

加入以下内容：

```bash
country=CN
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
network={
ssid="DHU-1X"
identity="190910228"
password="yangyu2002..."
eap=PEAP TTLS
phase1="peaplabel=0"
phase2="auth=GTC"
priority=999
disabled=0
auth_alg=OPEN
key_mgmt=WPA-EAP
}
```

### 3.尝试连接到目标网络

```bash
sudo wpa_supplicant -i wlan0 -c /etc/wpa_supplicant/wpa_supplicant.conf -B
```

### 4.用`dhclient`获取IP

```bash
sudo dhclient wlan0
```

大功告成！

## 附录A：wpa_supplicant 基本操作

[14168584.html](https://www.cnblogs.com/hokori/p/14168584.html)

### 1.启动连接

```bash
sudo wpa_supplicant -i wlan0 -c /etc/wpa_supplicant/wpa_supplicant.conf -B
```

> D - 驱动程序名称（可以是多个驱动程序：nl80211，wext）
i   - 接口名称
c   - 配置文件
B   - 在后台运行守护进程
> 

### 2. wpa_supplicant.conf 配置文件

示例：

```bash
ctrl_interface=/var/run/wpa_supplicant  # 一个目录,用于wpa_supplicant和wpa_cli的socket通信
network={
        ssid="xxx"		   # WIFI名称
        proto=WPA        # proto: list of accepted protocols, 可取WPA,RSN
                         # 如果没有设置，默认为: WPA RSN
        key_mgmt=WPA-PSK # 认证方式
                         # 如果没有设置，默认为: WPA-PSK WPA-EAP
        pairwise=TKIP    # 如果没有设置，默认为: CCMP TKIP
        group=TKIP       # 如果没有设置，默认为: CCMP TKIP WEP104 WEP40 
        psk="xxx"	   # WIFI密码
}
```

- 全部解析
  
    ```bash
    update_config=1      //是否允许wpa_supplicant更新（覆盖）配置
    eapol_version=1  //IEEE 802.1X / EAPOL版本
    ap_scan=1  //AP扫描/选择
    passive_scan=0   //是否强制被动扫描进行网络连接
    user_mpm=1   //MPM驻留
    max_peer_links=99  //最大对等链路数（0-255;默认值：99）
    mesh_max_inactivity=300   //检测STA不活动的超时（以秒为单位）（默认值：300秒）
    cert_in_cb=1  //cert_in_cb - 是否在事件中包含对等证书转储
    fast_reauth=1   //EAP快速重新认证
    driver_param="field=value"   //驱动程序接口参数
    country=US  //国家码
    dot11RSNAConfigSATimeout=60  //PMKSA的最长寿命，以秒为单位; 默认43200
    uuid=12345678-9abc-def0-1234-56789abcdef0  //设备的通用唯一标识符
    auto_uuid=0   //自动UUID行为
    device_name=Wireless Client  //设备名称
    manufacturer=Company  //生产厂家
    model_name=cmodel  //型号名称
    model_number=123 // 型号
    serial_number=12345  //序列号
    device_type=1-0050F204-1  //主要设备类型
    os_version=01020300    //操作系统版本
    config_methods=label virtual_display virtual_push_button keypad  //配置方法
    wps_cred_processing=0   //凭证处理
    wps_vendor_ext_m1=000137100100020001  //WPS M1中的供应商属性，例如，Windows 7垂直配对
    wps_nfc_dev_pw: Hexdump of Device Password     //WPS的NFC密码令牌
    wps_priority=0  //通过WPS添加网络的优先级
    bss_max_count=200   //要保留在内存中的最大BSS条目数
    filter_ssids=0  //filter_ssids - 基于SSID的扫描结果过滤
    p2p_disabled=1   //禁用P2P功能
    p2p_go_max_inactivity=300  //检测STA不活动的超时（以秒为单位）（默认值：300秒）
    p2p_passphrase_len=8   //P2P GO的密码长度
    p2p_search_delay=500   //并发P2P搜索迭代之间的额外延迟
    okc=0   //机会密钥缓存（也称为主动密钥缓存）默认
    pmf=0  //受保护的管理框架
    sae_groups=21 20 19 26 25  //按优先顺序启用SAE有限循环组
    dtim_period=2   //DTIM周期的默认值（如果未在网络块中覆盖）
    beacon_int=100   //Beacon间隔的默认值（如果未在网络块中覆盖）
    ap_vendor_elements=dd0411223301   //Beacon和Probe Response帧的其他供应商特定元素
    ignore_old_scan_res=0  //忽略比请求更早的扫描结果
    mac_addr=0  //MAC地址策略
    rand_addr_lifetime=60  //随机MAC地址的生命周期，以秒为单位（默认值：60）
    preassoc_mac_addr=0   //预关联操作的MAC地址策略（扫描，ANQP）
    gas_rand_mac_addr=0  //GAS操作的MAC地址策略
    gas_rand_addr_lifetime=60   //GAS随机MAC地址的生命周期（以秒为单位）
    interworking=1   //启用互通
    go_interworking=1   //启用互通的P2P GO广告
    go_access_network_type=0   //P2P GO互通：接入网络类型
    go_internet=1   //P2P GO互通：网络是否提供到Internet的连接
    go_venue_group=7  go_venue_type=1  //p2p-go互通：群组场馆信息（可选）
    hessid=00:11:22:33:44:55  //同源ESS标识符
    auto_interworking=0   //自动网络选择行为
    gas_address3=0  //GAS Address3字段行为
    ftm_responder=0  // 在扩展功能元素位70中发布精确定时测量（FTM）响应器功能。
    ftm_initiator=0  //在扩展功能元素位71中发布精确定时测量（FTM）启动器功能。
    mbo_cell_capa=3  //MBO蜂窝数据功能
    non_pref_chan=81:5:10:2 81:1:0:2 81:9:0:2  //多频段操作（MBO）非首选频道
     oce=1       //优化的连接体验（OCE）
    mem_only_psk=0        //mem_only_psk：是否仅在内存中保留PSK /密码
    
    ```
    

### 3. wpa_cli

```bash
sudo wpa_cli -i wlan0

#command
scan
scan_result
add_network   #return <network id>
set_network <network id> ssid "xxx"
set_network <network id> psk "xxx"
enable_network <network id>
save_config

#disconnect 
disable_network <network id>

#connect saved wifi
wpa_cli -i wlan0 list_network  
wpa_cli -i wlan0 select_network  <network id>
wpa_cli -i wlan0 enable_network  <network id>
wpa_cli -i wlan0 save_config
```

### 4.dhcp 获取 ip 地址

```bash
dhclient <wlan0>
```