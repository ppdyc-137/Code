```bash
ssh -L [bind_address:]port:host:hostport
```

Specifies that connections to the given TCP port on the local (client) host are to be forwarded to the given `host` and `port`, on the remote side.
bind_address是可选的，默认是0.0.0.0，表示所有接口上可见。也可以设置为localhost，只在本地可访问。

```bash
ssh -R [bind_address:]port:host:hostport 
```
 将远程服务器上的TCP端口的连接转发到本地，也就是访问`bind_address`的`port`相当于访问`host`的`hostport`。与上面不同的是这里的`bind_address`指的是你要连接到的那个服务器，后面的`host`和`hostport`指的才是本机。例如`ssh -R 8022:localhost:22 root@ppdy.space`，这才是将本地的22端口映射到ppdy.space的8022端口。
 默认情况下服务器上的TCP监听只绑定到服务器上的`lo`接口，也就是只有在服务器内的localhost才能访问。要让在公网上访问，需要在服务器上开启GatewayPorts这个选项,并且bind_address也要设置为公网地址。

端口转发随着ssh连接的停止而停止