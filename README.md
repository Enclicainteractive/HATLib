# HAT Format (Helix Audio Transform) Specification

![HAT logo](https://github.com/Enclicainteractive/HATLib/blob/main/Asset/images/Helix%20Audio%20Transform%20logo_light.png?raw=true)


The HAT file format is designed to support advanced audio features, including multi-channel audio, 3D spatial data, and efficient lossless compression. Below is a detailed specification of the HAT format.

## HAT File Format Specifications

| Field Name                 | Type                  | Description                                                                                           |
|----------------------------|-----------------------|-------------------------------------------------------------------------------------------------------|
| HAT_VERSION                | String (4 bytes)      | Version of the HAT format.                                                                            |
| HAT_CHANNELS               | Integer (4 bytes)     | Number of audio channels. Can be mono, stereo, or up to 32 channels.                                   |
| HAT_3D_SPATIAL_DATA        | Float array (3x4 bytes)| 3D spatial coordinates (x, y, z) indicating where the audio is played in 3D space.                    |
| HAT_COMPRESSION_RATIO      | Float (4 bytes)       | Ratio of compression applied to the audio data.                                                       |
| HAT_COMPRESSION_METHOD     | Enum (4 bytes)        | Compression method used (e.g., NONE, LOSSLESS).                                                        |
| TRACKS                     | Integer (4 bytes)     | Number of audio tracks in the file.                                                                   |
| SAMPLERATE                 | Integer (4 bytes)     | Audio sample rate (e.g., 44100 Hz).                                                                   |
| BITRATE                    | Integer (4 bytes)     | Audio bitrate (e.g., 128 kbps).                                                                       |
| LENGTH                     | Integer (4 bytes)     | Length of the audio data in samples.                                                                  |
| COMPRESSION_DATA_MARKERS   | Array of structs      | Marks where data is compressed and what needs to be restored.                                         |
| SEEKMARKER                 | Integer (4 bytes)     | Position in the audio where seeking is enabled.                                                       |
| ARTIST_DATA                | Struct                | Optional. Includes channel art, artist name, and description.                                          |
| DATA_LENGTH                | Integer (4 bytes)     | Used for decompression of the data required for decompression                                         |
| TRACK_NAME                 | String                | Optional. Name of the track, album, track number, etc.                                                 |
| TRACKNUMBER                | Integer (4 bytes)     | If there are multiple tracks, this indicates the track number.                                         |
| START OF HAT SAMPLE AUDIO TRACK DATA | Marker       | Indicates the start of the audio track data.                                                           |
| END OF AUDIO TRACK DATA    | Marker                | Marks the end of the audio track data.                                                                |
| HATEOF                     | Marker                | Indicates the end of the HAT file.(Invalidated now)                                                            |                                                                  |

### Field Descriptions

1. **HAT_VERSION**: A string that indicates the version of the HAT format. This helps in identifying the format version for compatibility purposes.
   
2. **HAT_CHANNELS**: Specifies the number of audio channels. This can range from mono (1 channel) to a maximum of 32 channels for advanced audio setups.

3. **HAT_3D_SPATIAL_DATA**: An array of three floating-point numbers representing the x, y, and z coordinates for 3D spatial audio playback.

4. **HAT_COMPRESSION_RATIO**: A floating-point number that indicates the ratio of compression applied. A higher ratio means stronger compression.

5. **HAT_COMPRESSION_METHOD**: An enumeration indicating the compression method used. Common values include:
   - NONE: No compression.
   - LOSSLESS: Lossless compression that retains the original audio quality.

6. **TRACKS**: The total number of audio tracks contained within the HAT file.

7. **SAMPLERATE**: The audio sample rate, typically measured in Hertz (Hz). Common values include 44100 Hz, 48000 Hz, etc.

8. **BITRATE**: The bitrate of the audio data, measured in kilobits per second (kbps).

9. **LENGTH**: The length of the audio data in samples.

10. **COMPRESSION_DATA_MARKERS**: An array that marks where the data is compressed within the file and what needs to be restored during decompression.

11. **SEEKMARKER**: Indicates a position in the audio data where seeking is allowed, useful for fast-forwarding or rewinding.

12. **ARTIST_DATA**: Optional field that can include:
    - **Channel Art**: Image or graphic associated with the track.
    - **Artist Name**: Name of the artist.
    - **Description**: Additional description or notes about the track.
    - **Data Length**: The length of the audio data for decompression.

13. **TRACK_NAME**: Optional field for the name of the track, album, and track number.

14. **TRACKNUMBER**: Indicates the track number if the file contains multiple tracks.

15. **START OF HAT SAMPLE AUDIO TRACK DATA**: A marker to indicate the beginning of the audio track data within the file.

16. **END OF AUDIO TRACK DATA**: A marker to indicate the end of the audio track data.

17. **HATEOF**: A marker indicating the end of the HAT file.

## Example Structure

Here is a simplified example structure of a HAT file:

```
+-----------------------------+
| HAT_VERSION                 |
+-----------------------------+
| HAT_CHANNELS                |
+-----------------------------+
| HAT_3D_SPATIAL_DATA         |
+-----------------------------+
| HAT_COMPRESSION_RATIO       |
+-----------------------------+
| HAT_COMPRESSION_METHOD      |
+-----------------------------+
| TRACKS                      |
+-----------------------------+
| SAMPLERATE                  |
+-----------------------------+
| BITRATE                     |
+-----------------------------+
| LENGTH                      |
+-----------------------------+
| COMPRESSION_DATA_MARKERS    |
+-----------------------------+
| SEEKMARKER                  |
+-----------------------------+
| ARTIST_DATA (optional)      |
+-----------------------------+
| TRACK_NAME (optional)       |
+-----------------------------+
| TRACKNUMBER                 |
+-----------------------------+
| START OF HAT SAMPLE AUDIO   |
+-----------------------------+
| AUDIO TRACK DATA            |
+-----------------------------+
| END OF AUDIO TRACK DATA     |
+-----------------------------+
| HATEOF                      |
+-----------------------------+
```

## Usage

### Encoding

To encode a WAV or OGG file into the HAT format, use the HATEncoder tool:

```
HATEncoder <input WAV/OGG file> <output HAT file>
```

### Decoding

To decode a HAT file and stream it to an audio player, use the HATDecoder tool:

```
HATPlayer <input HAT file>
```

### Player

The HATPlayer tool plays the HAT file, utilizing the 3D spatial data to create a 3D audio effect:

```
HATPlayer <input HAT file>
```

## Building the Projects

To build the projects, follow these steps:

1. Create a build directory at the same level as the project directories:

   ```
   mkdir build
   cd build
   ```

2. Build `HATEncoder` project:

   ```
   mkdir HATEncoder
   cd HATEncoder
   cmake ../../HATEncoder
   make
   cd ..
   ```

3. Build `HATPlayer` project:

   ```
   mkdir HATPlayer
   cd HATPlayer
   cmake ../../HATPlayer
   make
   cd ..
   ```
