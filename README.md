# OpenStrip

## Protocols

### Serial/MQTT Packet Format
250k 8n1
'OpenStrp', byte Command, byte[] data
Command:
  d - DMX data
  c - Config set
  g - Config get
  s - Status
