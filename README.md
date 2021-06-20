# target-ota-app

### Frame types:
```
  | start | length | data | end |
```
```
  | ack |
```
```
  | err |
```
```
  | start OTA |
```
```
  | OTA complete |
```

### Frames sizes:
```
| start | length | data | end |
```

* **start**:  1 byte
* **length**: 1 byte
* **data**:   \<length\> byte
* **end**:    1 byte


```
| ack |
```

* **ack**: 1 byte

```
| err |
```

* **err**: 1 byte


```
| start OTA |
```

* **start OTA**: ? byte

```
| OTA complete |
```

* **OTA complete**: 1 byte

### Communication flow:
```
 ______                                     ________ 
| Host |                                   | Target |
|______|                                   |________|
    |             | start OTA |                 |    
    |       ------------------------->          |    
    |                                           |    
    |                | ack |                    |    
    |       <------------------------           |    
    |                                           |    
    |                                           |    
    |    | start | length | data | end |        |    
    |       ------------------------>           |    
    |                                           |    
    |                | ack |                    |    
    |       <------------------------           |    
    |                   .                       |    
    |                   .                       |    
    |                   .                       |    
    |            | OTA complete |               |    
    |       <------------------------           |    
    |                                           |    
    |                                           |    
```
