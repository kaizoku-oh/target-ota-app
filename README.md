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
  | begin_ota |
```
```
  | finished_ota |
```

### Frames sizes:
```
| start | length | data | end |
```

* **start**:  1 byte
* **length**: 1 byte
* **data**:   \<length\> byte (max: 64 bytes)
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
| begin_ota |
```

* **begin_ota**: 1 byte

```
| finished_ota |
```

* **finished_ota**: 1 byte

### Communication flow:
```
 ______                                     ________ 
| Host |                                   | Target |
|______|                                   |________|
    |             | begin_ota |                 |    
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
    |            | finished_ota |               |    
    |       <------------------------           |    
    |                                           |    
    |                                           |    
```
