# iptrail

#### 介绍
For the time being, it is only for MIWiFi (AX6).
目前仅针对红米路由器AX6。

Track IP from your routers under DHCP, via email.
通过邮件监视路由器DHCP分配的WAN口IP地址。



#### 为什么要开发 iptrail
某高校一舍取消静态IP，改用DHCP。许多服务器因此报废。

我们需要一种稳定的手段来随时知悉路由器的IP地址，以便远程访问，恢复服务器的便捷性。

#### 依赖项
- Boost
  - Asio
  - Beast
  - Json
- OpenSSL

#### 开发进度
- [x] 使用 SMTP 发送任意邮件
- [ ] 获取路由器的 Token
- [ ] 向路由器发送 http 请求，获取 IP
- [ ] 监控 IP 变动，及时发送邮件到指定地址

---

朋友都看到这儿了，点个Star吧?9?1