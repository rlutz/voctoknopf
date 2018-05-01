Voctomix command protocol
-------------------------

Voctomix is controlled via TCP connections to port 9999 of the
computer running `voctocore`.  It implements a simple plain-text
protocol where one message equals one line.  There are three types of
messages:

* requests sent from a client to the server
* responses sent from the server to a client requesting a state
* broadcast messages sent from the server to all connected clients
  when the state changes

In this document, requests sent to the server are indicated by lines
starting with `->` and responses and broadcast messages sent to the
client are indicated by lines starting with `<-`.

### Reading the current state

```
-> get_stream_status
<- stream_status live
  or
<- stream_status blank pause
  or
<- stream_status blank nostream
```

```
-> get_composite_mode
<- composite_mode fullscreen
  or
<- composite_mode side_by_side_equal
  or
<- composite_mode side_by_side_preview
  or
<- composite_mode picture_in_picture
```

```
-> get_video
<- video_status slides cam2
  or some other combination of cam1, cam2, cam3, and slides
```

### Selecting a single source

```
-> set_videos_and_composite cam1 * fullscreen
<- composite_mode fullscreen
<- video_status cam1 cam2
```

The second value may differ and can safely be ignored.

### Selecting a composite source

```
-> set_videos_and_composite slides cam2 picture_in_picture
<- composite_mode picture_in_picture
<- video_status slides cam2
```

### Blanking the stream

```
-> set_stream_blank pause
<- stream_status blank pause
```

```
-> set_stream_blank nostream
<- stream_status blank nostream
```

```
-> set_stream_live
<- stream_status live
```

### Communicating with other connected clients

```
-> message Hello world!
<- message Hello world!
```

There are some additional commands which aren't relevant to this
project.  Typing `help` gives you a list with a short description for
each command.
