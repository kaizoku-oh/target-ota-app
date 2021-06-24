# target-ota-app

### Frame types:
Data frame:
```
  | data_start | length | data | data_end |
```
Acknowledgment frame:
```
  | ack |
```
Error frame:
```
  | err |
```
Begin frame:
```
  | begin_start | file size | begin_end |
```
Finish frame:
```
  | finished_ota |
```

### Frames sizes:
```
| data_start | length | data | data_end |
```

* **data_start**:  1 byte
* **length**:      1 byte
* **data**:        \<length\> byte (max: 64 bytes)
* **data_end**:    1 byte


```
| ack |
```

* **ack**: 1 byte

```
| err |
```

* **err**: 1 byte


```
  | begin_start | file size | begin_end |
```

* **begin_start**: 1 byte
* **file_size**:   2 byte
* **begin_end**:   1 byte

```
| finished_ota |
```

* **finished_ota**: 1 byte

### Communication flow:
```
 ______                                                ________ 
| Host |                                              | Target |
|______|                                              |________|
    |       | begin_start | file size | begin_end |        |    
    |             ------------------------->               |    
    |                                                      |    
    |                      | ack |                         |    
    |             <------------------------                |    
    |                                                      |    
    |                                                      |    
    |     | data_start | length | data | data_end |        |    
    |             ------------------------>                |    
    |                                                      |    
    |                      | ack |                         |    
    |             <------------------------                |    
    |                         .                            |    
    |                         .                            |    
    |                         .                            |    
    |                  | finished_ota |                    |    
    |             <------------------------                |    
    |                                                      |    
    |                                                      |    
```
