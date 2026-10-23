Yes. After checking the actual DNS queries and HTTP request paths in `57.pcapng`, I found several items worth investigating. Most traffic looks consistent with ordinary mobile-app and connectivity-check activity, but a few domains are unusual enough to investigate rather than automatically trust.

## 1. DNS names that stand out

`example.org`

Investigate context

30 DNS queries observed

This domain is reserved for documentation and examples, not an ordinary production website. Its appearance can be harmless, such as a test or application connectivity check, but repeated queries are worth correlating with the originating process or client.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://www.iana.org\&sz=32)

IANA

+1



`vc-brain.ndcpp-os.com`

Unverified

8 queries observed

This hostname is not immediately recognizable as a major consumer-service endpoint. The capture shows queries originating from `192.168.1.2`, with additional queries from the router. I cannot establish from the hostname alone which application owns it or whether it is malicious.

`ap-dw-collect.ksord.com`

Investigate context

12 queries observed

Queries originate from `192.168.1.11` and the router. The `collect` component of the hostname suggests a possible data-collection endpoint, but naming is not proof of telemetry, data exfiltration or malware. Public reputation information is limited and does not establish malicious activity.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://gridinsoft.com\&sz=32)

Gridinsoft LLC

+1



`teekka.com`

Unverified

6 queries observed

This is another domain to correlate with its source device and destination IP. I do not have sufficient evidence to label it malicious or identify the responsible application.

`conn-service-us-04.allawnos.com` and `conn-service-us-05.allawnos.com`

Known association

97 DNS queries combined

These are associated with OPPO device services and use Akamai infrastructure. They are not inherently suspicious. Their high frequency is consistent with device connectivity or background service activity.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://www.netify.ai\&sz=32)

OPPO

+1



Other domains worth identifying by process or device include `sg-o-api-server.allsaints.tv`, `vc-brain.ndcpp-os.com`, and `doh.cleanbrowsing.org`. The last is associated with a DNS-over-HTTPS service, so its presence is not automatically suspicious.

## 2. HTTP requests and URLs that deserve attention

These are the actual HTTP request paths identified in the capture.

| URL / endpoint                                       | Client         | Assessment                                                                         |
| ---------------------------------------------------- | -------------- | ---------------------------------------------------------------------------------- |
| `http://192.168.1.1/cgi-bin/ajax`                    | `192.168.1.13` | Highest priority: router management requests, including session-related parameters |
| `http://c.whatsapp.net/chat`                         | `192.168.1.2`  | Investigate the POST request and inspect its complete TCP stream                   |
| `http://192.168.1.16:8008/apps/YouTube`              | `192.168.1.11` | Likely local media-device control                                                  |
| `http://192.168.1.16:8008/ssdp/device-desc.xml`      | `192.168.1.11` | Device-description XML; inspect for device name and model                          |
| `http://www.google.us/generate_204`                  | `192.168.1.4`  | Normal connectivity check                                                          |
| `http://connectivitycheck.gstatic.com/generate_204`  | `192.168.1.4`  | Normal connectivity check                                                          |
| `http://conn-service-us-04.allawnos.com/generate204` | `192.168.1.4`  | Likely device connectivity check                                                   |

### The two most interesting HTTP findings

1\. `c.whatsapp.net/chat`

The capture contains a POST request from `192.168.1.2` to the WhatsApp-related hostname. This deserves a closer look because the request is visible as HTTP rather than ordinary HTTPS.

That does not automatically mean WhatsApp messages or credentials were exposed. Inspect the complete stream to determine whether it is an actual application request, a protocol-specific exchange, or an incomplete interpretation of the traffic.

2\. Router session-related requests

The client `192.168.1.13` repeatedly accesses `/cgi-bin/ajax`, including requests involving `get_refresh_sessionid` and `get_lan_status`. This is more directly relevant to the security of the local network because the administration interface uses HTTP.

## 3. Wireshark filters for the suspicious items

Try these filters to narrow down the investigation.

Inspect the WhatsApp-related request

```
http.host == "c.whatsapp.net"
```

Copy filter

Find queries for the unfamiliar domains

```
dns.qry.name == "example.org" ||
dns.qry.name == "teekka.com" ||
dns.qry.name == "vc-brain.ndcpp-os.com" ||
dns.qry.name == "ap-dw-collect.ksord.com"
```

Copy filter

Inspect DNS errors

```
dns.flags.rcode != 0
```

Copy filter

Look for unusually large DNS queries

```
dns && udp.length > 300
```

This is a triage filter, not a malware detector. Large DNS messages can be entirely legitimate.

Copy filter

For each unfamiliar domain, check the DNS response, resolved IP, client source, timestamp and subsequent TCP/TLS connections. If possible, correlate the domain with the application running on that device.

## Verdict

- Confirmed malicious website: None established from the observed names and paths alone.
- Unusual domains: `vc-brain.ndcpp-os.com`, `ap-dw-collect.ksord.com`, `teekka.com`, and `example.org` deserve contextual investigation.
- Potentially sensitive HTTP: The router administration requests and the POST to `c.whatsapp.net/chat` are the most important streams to inspect.
- Likely routine activity: Google connectivity checks, YouTube media delivery, and the OPPO-associated connectivity endpoints.

The next best step is to inspect the full TCP stream for `c.whatsapp.net/chat` and the router's AJAX requests. Those streams are much more likely to answer whether anything sensitive was actually transmitted than the domain names alone.Yes. After checking the actual DNS queries and HTTP request paths in `57.pcapng`, I found several items worth investigating. Most traffic looks consistent with ordinary mobile-app and connectivity-check activity, but a few domains are unusual enough to investigate rather than automatically trust.

## 1. DNS names that stand out

`example.org`

Investigate context

30 DNS queries observed

This domain is reserved for documentation and examples, not an ordinary production website. Its appearance can be harmless, such as a test or application connectivity check, but repeated queries are worth correlating with the originating process or client.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://www.iana.org\&sz=32)

IANA

+1



`vc-brain.ndcpp-os.com`

Unverified

8 queries observed

This hostname is not immediately recognizable as a major consumer-service endpoint. The capture shows queries originating from `192.168.1.2`, with additional queries from the router. I cannot establish from the hostname alone which application owns it or whether it is malicious.

`ap-dw-collect.ksord.com`

Investigate context

12 queries observed

Queries originate from `192.168.1.11` and the router. The `collect` component of the hostname suggests a possible data-collection endpoint, but naming is not proof of telemetry, data exfiltration or malware. Public reputation information is limited and does not establish malicious activity.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://gridinsoft.com\&sz=32)

Gridinsoft LLC

+1



`teekka.com`

Unverified

6 queries observed

This is another domain to correlate with its source device and destination IP. I do not have sufficient evidence to label it malicious or identify the responsible application.

`conn-service-us-04.allawnos.com` and `conn-service-us-05.allawnos.com`

Known association

97 DNS queries combined

These are associated with OPPO device services and use Akamai infrastructure. They are not inherently suspicious. Their high frequency is consistent with device connectivity or background service activity.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://www.netify.ai\&sz=32)

OPPO

+1



Other domains worth identifying by process or device include `sg-o-api-server.allsaints.tv`, `vc-brain.ndcpp-os.com`, and `doh.cleanbrowsing.org`. The last is associated with a DNS-over-HTTPS service, so its presence is not automatically suspicious.

## 2. HTTP requests and URLs that deserve attention

These are the actual HTTP request paths identified in the capture.

| URL / endpoint                                       | Client         | Assessment                                                                         |
| ---------------------------------------------------- | -------------- | ---------------------------------------------------------------------------------- |
| `http://192.168.1.1/cgi-bin/ajax`                    | `192.168.1.13` | Highest priority: router management requests, including session-related parameters |
| `http://c.whatsapp.net/chat`                         | `192.168.1.2`  | Investigate the POST request and inspect its complete TCP stream                   |
| `http://192.168.1.16:8008/apps/YouTube`              | `192.168.1.11` | Likely local media-device control                                                  |
| `http://192.168.1.16:8008/ssdp/device-desc.xml`      | `192.168.1.11` | Device-description XML; inspect for device name and model                          |
| `http://www.google.us/generate_204`                  | `192.168.1.4`  | Normal connectivity check                                                          |
| `http://connectivitycheck.gstatic.com/generate_204`  | `192.168.1.4`  | Normal connectivity check                                                          |
| `http://conn-service-us-04.allawnos.com/generate204` | `192.168.1.4`  | Likely device connectivity check                                                   |

### The two most interesting HTTP findings

1\. `c.whatsapp.net/chat`

The capture contains a POST request from `192.168.1.2` to the WhatsApp-related hostname. This deserves a closer look because the request is visible as HTTP rather than ordinary HTTPS.

That does not automatically mean WhatsApp messages or credentials were exposed. Inspect the complete stream to determine whether it is an actual application request, a protocol-specific exchange, or an incomplete interpretation of the traffic.

2\. Router session-related requests

The client `192.168.1.13` repeatedly accesses `/cgi-bin/ajax`, including requests involving `get_refresh_sessionid` and `get_lan_status`. This is more directly relevant to the security of the local network because the administration interface uses HTTP.

## 3. Wireshark filters for the suspicious items

Try these filters to narrow down the investigation.

Inspect the WhatsApp-related request

```
http.host == "c.whatsapp.net"
```

Copy filter

Find queries for the unfamiliar domains

```
dns.qry.name == "example.org" ||
dns.qry.name == "teekka.com" ||
dns.qry.name == "vc-brain.ndcpp-os.com" ||
dns.qry.name == "ap-dw-collect.ksord.com"
```

Copy filter

Inspect DNS errors

```
dns.flags.rcode != 0
```

Copy filter

Look for unusually large DNS queries

```
dns && udp.length > 300
```

This is a triage filter, not a malware detector. Large DNS messages can be entirely legitimate.

Copy filter

For each unfamiliar domain, check the DNS response, resolved IP, client source, timestamp and subsequent TCP/TLS connections. If possible, correlate the domain with the application running on that device.

## Verdict

- Confirmed malicious website: None established from the observed names and paths alone.
- Unusual domains: `vc-brain.ndcpp-os.com`, `ap-dw-collect.ksord.com`, `teekka.com`, and `example.org` deserve contextual investigation.
- Potentially sensitive HTTP: The router administration requests and the POST to `c.whatsapp.net/chat` are the most important streams to inspect.
- Likely routine activity: Google connectivity checks, YouTube media delivery, and the OPPO-associated connectivity endpoints.

The next best step is to inspect the full TCP stream for `c.whatsapp.net/chat` and the router's AJAX requests. Those streams are much more likely to answer whether anything sensitive was actually transmitted than the domain names alone.Yes. After checking the actual DNS queries and HTTP request paths in `57.pcapng`, I found several items worth investigating. Most traffic looks consistent with ordinary mobile-app and connectivity-check activity, but a few domains are unusual enough to investigate rather than automatically trust.

## 1. DNS names that stand out

`example.org`

Investigate context

30 DNS queries observed

This domain is reserved for documentation and examples, not an ordinary production website. Its appearance can be harmless, such as a test or application connectivity check, but repeated queries are worth correlating with the originating process or client.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://www.iana.org\&sz=32)

IANA

+1



`vc-brain.ndcpp-os.com`

Unverified

8 queries observed

This hostname is not immediately recognizable as a major consumer-service endpoint. The capture shows queries originating from `192.168.1.2`, with additional queries from the router. I cannot establish from the hostname alone which application owns it or whether it is malicious.

`ap-dw-collect.ksord.com`

Investigate context

12 queries observed

Queries originate from `192.168.1.11` and the router. The `collect` component of the hostname suggests a possible data-collection endpoint, but naming is not proof of telemetry, data exfiltration or malware. Public reputation information is limited and does not establish malicious activity.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://gridinsoft.com\&sz=32)

Gridinsoft LLC

+1



`teekka.com`

Unverified

6 queries observed

This is another domain to correlate with its source device and destination IP. I do not have sufficient evidence to label it malicious or identify the responsible application.

`conn-service-us-04.allawnos.com` and `conn-service-us-05.allawnos.com`

Known association

97 DNS queries combined

These are associated with OPPO device services and use Akamai infrastructure. They are not inherently suspicious. Their high frequency is consistent with device connectivity or background service activity.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://www.netify.ai\&sz=32)

OPPO

+1



Other domains worth identifying by process or device include `sg-o-api-server.allsaints.tv`, `vc-brain.ndcpp-os.com`, and `doh.cleanbrowsing.org`. The last is associated with a DNS-over-HTTPS service, so its presence is not automatically suspicious.

## 2. HTTP requests and URLs that deserve attention

These are the actual HTTP request paths identified in the capture.

| URL / endpoint                                       | Client         | Assessment                                                                         |
| ---------------------------------------------------- | -------------- | ---------------------------------------------------------------------------------- |
| `http://192.168.1.1/cgi-bin/ajax`                    | `192.168.1.13` | Highest priority: router management requests, including session-related parameters |
| `http://c.whatsapp.net/chat`                         | `192.168.1.2`  | Investigate the POST request and inspect its complete TCP stream                   |
| `http://192.168.1.16:8008/apps/YouTube`              | `192.168.1.11` | Likely local media-device control                                                  |
| `http://192.168.1.16:8008/ssdp/device-desc.xml`      | `192.168.1.11` | Device-description XML; inspect for device name and model                          |
| `http://www.google.us/generate_204`                  | `192.168.1.4`  | Normal connectivity check                                                          |
| `http://connectivitycheck.gstatic.com/generate_204`  | `192.168.1.4`  | Normal connectivity check                                                          |
| `http://conn-service-us-04.allawnos.com/generate204` | `192.168.1.4`  | Likely device connectivity check                                                   |

### The two most interesting HTTP findings

1\. `c.whatsapp.net/chat`

The capture contains a POST request from `192.168.1.2` to the WhatsApp-related hostname. This deserves a closer look because the request is visible as HTTP rather than ordinary HTTPS.

That does not automatically mean WhatsApp messages or credentials were exposed. Inspect the complete stream to determine whether it is an actual application request, a protocol-specific exchange, or an incomplete interpretation of the traffic.

2\. Router session-related requests

The client `192.168.1.13` repeatedly accesses `/cgi-bin/ajax`, including requests involving `get_refresh_sessionid` and `get_lan_status`. This is more directly relevant to the security of the local network because the administration interface uses HTTP.

## 3. Wireshark filters for the suspicious items

Try these filters to narrow down the investigation.

Inspect the WhatsApp-related request

```
http.host == "c.whatsapp.net"
```

Copy filter

Find queries for the unfamiliar domains

```
dns.qry.name == "example.org" ||
dns.qry.name == "teekka.com" ||
dns.qry.name == "vc-brain.ndcpp-os.com" ||
dns.qry.name == "ap-dw-collect.ksord.com"
```

Copy filter

Inspect DNS errors

```
dns.flags.rcode != 0
```

Copy filter

Look for unusually large DNS queries

```
dns && udp.length > 300
```

This is a triage filter, not a malware detector. Large DNS messages can be entirely legitimate.

Copy filter

For each unfamiliar domain, check the DNS response, resolved IP, client source, timestamp and subsequent TCP/TLS connections. If possible, correlate the domain with the application running on that device.

## Verdict

- Confirmed malicious website: None established from the observed names and paths alone.
- Unusual domains: `vc-brain.ndcpp-os.com`, `ap-dw-collect.ksord.com`, `teekka.com`, and `example.org` deserve contextual investigation.
- Potentially sensitive HTTP: The router administration requests and the POST to `c.whatsapp.net/chat` are the most important streams to inspect.
- Likely routine activity: Google connectivity checks, YouTube media delivery, and the OPPO-associated connectivity endpoints.

The next best step is to inspect the full TCP stream for `c.whatsapp.net/chat` and the router's AJAX requests. Those streams are much more likely to answer whether anything sensitive was actually transmitted than the domain names alone.
