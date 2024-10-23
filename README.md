# ESPHome Luxtronik V1
> [!NOTE]
> This is the German version, for the English version, scroll down or click [here](#esphome-luxtronik-v1-english).

Luxtronik V1 ist eine ESPHome-Komponente zur Erstellung einer ESP-Firmware, die die Integration eines Luxtronik V1 Heizungssteuergeräts in das Smart Home ermöglicht. Die Komponente ist primär für die Einbindung in Home Assistant gedacht, aber über MQTT kann auch eine Integration mit einer alternativen Hausautomatisierungs-Software realisiert werden. Die Luxtronik Heizungsregelung in der Version 1 (V1) verfügt nicht über eine Netzwerkschnittstelle, sondern stellt lediglich eine RS-232-Schnittstelle zur Verfügung. Daher ist ein Mikrocontroller als Gateway zwischen Heizungssteuergerät und Netzwerk notwendig.

Dieses Projekt wurde stark von der [Luxtronik V1 ESPHome-Komponente](https://github.com/CBrosius/luxtronik_v1) von [CBrosius](https://github.com/CBrosius) inspiriert. Vielen Dank an CBrosius für die großartige Arbeit, die mir sehr geholfen hat, die Luxtronik-Schnittstelle und die Arbeitsweise von ESPHome-Komponenten zu verstehen.

- [Haftungsausschluss](#haftungsausschluss)
- [Lizenz](LICENSE)
- [Hardware-Aufbau](#hardware-aufbau)
- [Software-Konfiguration](#software-konfiguration)
  - [Luxtronik-Komponente](#luxtronik-komponente)
  - [UART-Komponente](#uart-komponente)
  - [Time-Komponente](#time-komponente)
  - [API/MQTT-Komponente](#apimqtt-komponente)
  - [WiFi-Komponente](#wifi-komponente)
  - [Sensoren](#sensoren)
    - [Numerische Sensoren](#numerische-sensoren)
    - [Binäre Sensoren](#binäre-sensoren)
    - [Textsensoren](#textsensoren)
- [Luxtronik-Konfiguration](#luxtronik-konfiguration)
- [Hilfe/Unterstützung](SUPPORT.md)
- [Mitwirkung](CONTRIBUTING.md)
- [Bekannte Probleme](https://github.com/jensrossbach/esphome-luxtronik-v1/issues?q=is%3Aissue+is%3Aopen+label%3A%22known+issue%22)

## Haftungsausschluss
**DIESE SOFTWARE UND DEREN AUTOR STEHEN IN KEINER VERBINDUNG ZU AIT / ALPHA INNOTEC.**

**DIE SOFTWARE (EINSCHLIEßLICH DER DOKUMENTATION MIT HARDWARE BEISPIEL-AUFBAUTEN) WIRD OHNE MÄNGELGEWÄHR UND OHNE JEGLICHE AUSDRÜCKLICHE ODER STILLSCHWEIGENDE GEWÄHRLEISTUNG, EINSCHLIEẞLICH, ABER NICHT BESCHRÄNKT AUF DIE GEWÄHRLEISTUNG DER MARKTGÄNGIGKEIT, DER EIGNUNG FÜR EINEN BESTIMMTEN ZWECK UND DER NICHTVERLETZUNG VON RECHTEN DRITTER, ZUR VERFÜGUNG GESTELLT. DIE AUTOREN ODER URHEBERRECHTSINHABER SIND IN KEINEM FALL HAFTBAR FÜR ANSPRÜCHE, SCHÄDEN ODER ANDERE VERPFLICHTUNGEN, OB IN EINER VERTRAGS- ODER HAFTUNGSKLAGE, EINER UNERLAUBTEN HANDLUNG ODER ANDERWEITIG, DIE SICH AUS ODER IN VERBINDUNG MIT DER SOFTWARE ODER DER NUTZUNG ODER ANDEREN GESCHÄFTEN MIT DER SOFTWARE ERGEBEN.**

## Hardware-Aufbau
Obwohl sich dieses Projekt hauptsächlich auf die Software-Implementierung konzentriert, habe ich auch einen möglichen Hardware-Aufbau dokumentiert. Dabei gibt es verschiedenen Möglichkeiten für den Aufbau, insbesondere in Hinblick auf den verwendeten Mikrocontroller (z.B. ESP8266 oder ESP32) und die Art und Weise, wie der Mikrocontroller mit dem Luxtronik Heizungssteuergerät verbunden wird.

> [!TIP]
> Grundsätzlich kann sowohl ein ESP8266 als auch ein ESP32 verwendet werden. Letzterer ist dabei besser geeignet, da er über mehrere Hardware-UARTs verfügt. Der ESP8266 verfügt dagegen nur über einen einzigen bidirektionalen Hardware-UART, der standardmäßig für das Logging über USB verwendet wird. Bei Verwendung eines ESP8266 muss man daher entweder auf das USB-Logging verzichten oder Software-UART verwenden (welches unzuverlässiger ist).

Der ESP-Mikrocontroller kann aufgrund der unterschiedlichen Spannungspegel nicht direkt an die RS-232-Schnittstelle des Luxtronik Heizungssteuergeräts angeschlossen werden. Stattdessen wird ein MAX3232-IC benötigt, um zwischen den unterschiedlichen Spannungspegeln auf Luxtronik- und Mikrocontroller-Seite zu konvertieren. Es gibt fertige Seriell-zu-TTL Konvertierungsmodulplatinen, die mit einem MAX3232-IC und einem DE-9 (auch D-Sub 9) Stecker ausgestattet sind. Auf der RS-232-Seite haben diese Module einen (meist weiblichen) DE-9-Stecker (an dem aber nur GND, RX und TX verbunden sind) und auf der TTL-Seite haben sie 4 Pins - VCC und GND für die Stromversorgung des Chips sowie RX und TX für die Datenübertragung. Diese Pins müssen mit den entsprechenden Pins des Mikrocontrollers verbunden werden. Der folgende Steckplatinen-Schaltplan zeigt ein Beispiel für einen Versuchsaufbau mit einem ESP32 NodeMCU Entwicklungsboard als Mikrocontroller.

> [!IMPORTANT]
> Verbinde den VCC-Pin mit dem **3V3-Pin** des ESP-Mikrocontrollers und **nicht** mit dem 5V-Pin. Dadurch wird sichergestellt, dass die RX- und TX-Pins ebenfalls einen Spannungspegel von 3,3V haben und die GPIO-Pins des Mikrocontrollers nicht beschädigt werden.

![Steckplatinen-Schaltplan](img/breadboard_schematic.png)

Normalerweise möchte man den Mikrocontroller außerhalb des Wärmepumpengehäuses platzieren, um einen besseren Wi-Fi-Empfang und eine einfachere Wartung zu ermöglichen. Es gibt zwei Möglichkeiten, wo die „lange“ Verkabelung zwischen dem Mikrocontroller-Aufbau und der Luxtronik platziert werden kann:

1. Serielles Kabel (fertig oder selbst gebaut), das zwischen dem MAX3232-Modul und der RS-232-Schnittstelle der Luxtronik angeschlossen wird
2. 4-adriges Kabel zwischen dem Mikrocontroller und dem MAX3232-Modul (letzteres wird direkt an die RS-232-Schnittstelle der Luxtronik angeschlossen)

Die meisten im Internet beschriebenen Aufbauten verwenden Option 1, aber ich habe mich in meinem Fall für Option 2 entschieden. Als Kabel habe ich ein handelsübliches 4-adriges Telefonverlegekabel verwendet. Diese Kabel haben zwei verdrillte Adernpaare (rot + schwarz und weiß + gelb). Die roten und schwarzen Adern sind prädestiniert für VCC bzw. GND. Ich habe dann gelb für RX und weiß für TX gewählt (dieselbe Farbgebung wie im Schaltplan). Die Adern haben jeweils einen Durchmesser von 0,6 mm, sodass der Spannungsabfall bei 3,3V und der sehr geringen Stromstärke bei einer Kabellänge von 2-4 Metern vernachlässigbar ist.

Warum habe ich mich für Option 2 entschieden? Der Grund war, dass fertige serielle Kabel heutzutage schwer zu bekommen und teuer sind und - der wichtigere Grund - die großen DE-9-Steckerenden nicht durch die kleinen Durchlässe im Gehäuse der Wärmepumpe passen. Außerdem wollte ich mir kein eigenes Kabel bauen, denn auch DE-9-Stecker sind schwer zu bekommen und teuer. Für Option 2 habe ich die Drähte an einem Ende des Kabels direkt an die TTL-Pins der MAX3232-Modulplatine gelötet und diese dann direkt an die Luxtronik RS-232-Schnittstelle angeschlossen (mit einem kleinen Nullmodem-Adapter dazwischen, siehe Tipp unten). Für den Mikrocontroller-Aufbau habe ich Buchsenleisten sowie Leiterplatten-Schraubklemmenblöcke auf eine Lochrasterplatine gelötet und die entsprechenden Pins angeschlossen. An die Buchsenleisten habe ich eine ESP32 NodeMCU-Entwicklungsplatine angeschlossen und konnte dann die Drähte des Telefonkabels einfach an die Schraubklemmen anschließen, nachdem ich das Kabel durch die Durchlässe des Wärmepumpengehäuses geführt hatte.

> [!TIP]
> Die meisten Quellen im Internet geben an, dass das serielle Kabel zur Verbindung des Empfängers mit der Luxtronik-Heizungssteuerung ein Kabel mit einer 1:1-Belegung der RX- und TX-Pins sein muss. Dies scheint jedoch von der seriellen Schnittstelle abzuhängen, die auf der Empfängerseite verwendet wird. In meinem Fall musste ich einen Nullmodem-Adapter verwenden, der die RX- und TX-Pins vertauscht, um das Ganze zum Laufen zu bringen.

Es gibt bereits ähnliche Projekte zum Auslesen der seriellen Schnittstelle des Luxtronik V1 Heizungssteuergerätes. Diese Projekte haben gute Dokumentationen der erforderlichen Hardware-Einrichtung, also schaue dir bitte auch diese hilfreichen Ressourcen im Internet an:

- https://github.com/CBrosius/luxtronik_v1
- https://wiki.fhem.de/wiki/Luxtronik_1_in_FHEM (nicht ESPHome-basiert)

## Software-Konfiguration
Um eine ESPHome-Firmware zu erstellen, muss eine YAML-basierte Konfigurationsdatei erstellt werden. Du kannst die in diesem Repository bereitgestellte [Beispielkonfigurationsdatei](example_config/luxtronik_lwc.yaml) als Ausgangspunkt verwenden und sie an deine Bedürfnisse anpassen. Weitere Informationen zum Schreiben von ESPHome-Firmware-Konfigurationsdateien findest du in der [ESPHome-Dokumentation](https://www.esphome.io).

Die folgenden Abschnitte beschreiben die wichtigsten Komponenten, die in der Firmware-Konfigurationsdatei enthalten sind.

### Luxtronik-Komponente
Die Komponente Luxtronik-V1 ist unabdingbar und muss hinzugefügt werden, um ihre Sensoren zu verwenden.

Da es sich um eine individuelle Komponente handelt, die nicht Teil von ESPHome ist, muss sie explizit importiert werden. Am einfachsten ist es, die Komponente direkt aus diesem Repository zu laden.

##### Beispiel

```yaml
external_components:
  - source: github://jensrossbach/esphome-luxtronik-v1
    components: [luxtronik_v1]
```

Die folgenden generischen Einstellungen können konfiguriert werden:

| Option | Benötigt | Standardwert | Beschreibung |
| ------ | --------- | ------------- |------------ |
| `uart_id` | ja | - | ID der konfigurierten UART-Komponente (siehe unten) |
| `update_interval` | nein | 60s | Das Intervall, in dem die Komponente Daten vom Heizungssteuergerät abruft |

##### Beispiel
```yaml
luxtronik_v1:
  uart_id: uart_bus
  update_interval: 300s  # Aktualisierung alle 5 Minuten
```

### UART-Komponente
Eine [UART-Komponente](https://www.esphome.io/components/uart.html) ist für die korrekte Funktion der Luxtronik-Komponente zwingend erforderlich und muss daher immer hinzugefügt werden. Die seriellen Kommunikationsparameter (Baudrate, Datenbits, Stopbits und Parität) sind durch die Luxtronik V1 Heizungssteuerung fest vorgegeben und dürfen daher nicht verändert werden. Lediglich die RX- und TX-Pins können in Abhängigkeit von der verwendeten Hardware angepasst werden.

| Parameter | Wert         |
| --------- | ------------ |
| Baudrate  | 57600 Bits/s |
| Datenbits | 8            |
| Stopbits  | 1            |
| Parität   | Keine        |

##### Beispiel
Nachfolgend ein Beispiel für ein ESP32 NodeMCU, das den UART2 verwendet:

```yaml
uart:
  id: uart_bus
  rx_pin: GPIO16    # UART2 RX
  tx_pin: GPIO17    # UART2 TX
  baud_rate: 57600
  data_bits: 8
  stop_bits: 1
  parity: NONE
```

### Time-Komponente
Eine [Time-Komponente](https://www.esphome.io/components/time) ist nicht unbedingt erforderlich, wird aber empfohlen, wenn du die Fehler- und Abschaltungssensoren verwenden möchtest. Diese Sensoren liefern Zeitstempel, und damit die richtige Zeitzone verwendet wird, muss eine Time-Komponente hinzugefügt werden.

##### Beispiel
Nachfolgend ein Beispiel, das Home Assistant als Zeitquelle verwendet:

```yaml
time:
  - platform: homeassistant
    id: ha_time
```

### API/MQTT-Komponente
Eine [API-Komponente](https://www.esphome.io/components/api.html) ist erforderlich, wenn der ESP in Home Assistant integriert werden soll. Für den Fall, dass eine alternative Hausautomatisierungs-Software verwendet werden soll, muss stattdessen eine [MQTT-Komponente](https://www.esphome.io/components/mqtt.html) hinzugefügt werden.

##### Beispiel
Nachfolgend ein Beispiel für die Integration mit Home Assistant (und verschlüsselter API):

```yaml
api:
  encryption:
    key: !secret ha_api_key
```

Und hier ein Beispiel für die Verwendung mit einer alternativen Hausautomatisierungs-Software mittels MQTT:

```yaml
mqtt:
  broker: 10.0.0.2
  username: !secret mqtt_user
  password: !secret mqtt_password
```

### WiFi-Komponente
Eine [WiFi-Komponente](https://www.esphome.io/components/wifi.html) sollte vorhanden sein, da die Sensor-Werte ansonsten nicht ohne weiteres an ein anderes Gerät übertragen werden können.

##### Beispiel

```yaml
wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
```

### Sensoren
Die Luxtronik-Komponente verfügt über verschiedene Sensoren, welche Daten von der Heizungssteuerung ausgeben. Alle Sensoren sind optional und können weggelassen werden, wenn sie nicht benötigt werden. Je nach Art der von der Luxtronik-Heizungssteuerung gelieferten Daten werden entweder numerische, binäre oder Textsensoren verwendet, um die Werte auszugeben.

#### Numerische Sensoren
Die folgenden numerischen Sensoren können konfiguriert werden:

| Sensor | Geräteklasse | Beschreibung |
| ------ | ------------ | ------------ |
| `flow_temperature` | Temperatur | Ist-Temperatur Vorlauf Heizkreis |
| `return_temperature` | Temperatur | Ist-Temperatur Rücklauf Heizkreis |
| `return_set_temperature` | Temperatur | Soll-Temperatur Rücklauf Heizkreis |
| `hot_gas_temperature` | Temperatur | Temperatur Heißgas |
| `outside_temperature` | Temperatur | Außentemperatur |
| `hot_water_temperature` | Temperatur | Ist-Temperatur Brauchwarmwasser |
| `hot_water_set_temperature` | Temperatur | Soll-Temperatur Brauchwarmwasser |
| `heat_source_input_temperature` | Temperatur | Temperatur Wärmequelleneintritt |
| `heat_source_output_temperature` | Temperatur | Temperatur Wärmequellenaustritt |
| `mixed_circuit_1_temperature` | Temperatur | Ist-Temperatur Vorlauf Mischkreis 1 |
| `mixed_circuit_1_set_temperature` | Temperatur | Soll-Temperatur Vorlauf Mischkreis 1 |
| `remote_adjuster_temperature` | Temperatur | Temperatur Raumfernversteller |

Detaillierte Informationen zu den Konfigurationsmöglichkeiten der einzelnen Elemente findest du in der Dokumentation der [ESPHome Sensorkomponenten](https://www.esphome.io/components/sensor).

##### Beispiel
```yaml
sensor:
  - platform: luxtronik_v1
    flow_temperature:
      name: Ist-Temperatur Vorlauf Heizkreis
    return_temperature:
      name: Ist-Temperatur Rücklauf Heizkreis
    return_set_temperature:
      name: Soll-Temperatur Rücklauf Heizkreis
```

#### Binäre Sensoren
Die folgenden binären Sensoren können konfiguriert werden:

| Sensor | Geräteklasse | Beschreibung |
| ------ | ------------ | ------------ |
| `defrost_brine_flow` | - | Abtau, Soledruck, Durchfluss |
| `power_provider_lock_period` | Schloss | Sperrzeit EVU |
| `low_pressure_state` | Problem | Niederdruckpressostat |
| `high_pressure_state` | Problem | Hodruckpressostat |
| `engine_protection` | Problem | Motorschutz |
| `external_power` | - | Fremdstromanode |
| `defrost_valve` | - | Abtauventil |
| `hot_water_pump` | Laufen | Brauchwarmwasserumwälzpumpe |
| `heating_pump` | Laufen | Heizungsumwälzpumpe |
| `floor_heating_pump` | Laufen | Fußbodenheizungsumwälzpumpe |
| `housing_ventilation` | Laufen | Ventilation Wärmepumpengehäuse |
| `ventilation_pump` | Laufen | Ventilator, Brunnen- oder Soleumwälzpumpe |
| `compressor_1` | Laufen | Verdichter 1 in Wärmepumpe |
| `compressor_2` | Laufen | Verdichter 2 in Wärmepumpe |
| `extra_pump` | Laufen | Zusatzumwälzpumpe - Zirkulationspumpe |
| `secondary_heater_1` | Laufen | Zweiter Wärmeerzeuger 1 |
| `secondary_heater_2_failure` | Problem | Zweiter Wärmeerzeuger 2 - Sammelstörung |

Desweiteren kann folgender diagnostischer Binärsensor konfiguriert werden:

| Sensor | Geräteklasse | Beschreibung |
| ------ | ------------ | ------------ |
| `device_communication` | Problem | Zeigt an, ob die Kommunikation mit dem Heizungssteuergerät funktioniert |

Detaillierte Informationen zu den Konfigurationsmöglichkeiten der einzelnen Elemente findest du in der Dokumentation der [ESPHome Binärsensorkomponenten](https://www.esphome.io/components/binary_sensor).

##### Beispiel
```yaml
binary_sensor:
  - platform: luxtronik_v1
    hot_water_pump:
      name: Brauchwarmwasserumwälzpumpe
    floor_heating_pump:
      name: Fußbodenheizungsumwälzpumpe
    ventilation_pump:
      name: Ventilator
    compressor_1:
      name: Verdichter
    power_provider_lock_period:
      name: Sperrzeit EVU
    device_communication:
      name: Gerätekommunikation
```

#### Textsensoren
Die folgenden Textsensoren können konfiguriert werden:

| Sensor | Geräteklasse | Beschreibung |
| ------ | ------------ | ------------ |
| `device_type` | - | Wärmepumpentyp |
| `firmware_version` | - | Software-Stand Firmware |
| `bivalence_level` | - | Bivalenzstufe |
| `operational_state` | - | Betriebszustand |
| `heating_mode` | - | Betriebsart Heizung |
| `hot_water_mode` | - | Betriebsart Brauchwarmwasser |
| `mixer_1_state` | - | Zustand Mischer 1 |
| `error_1_code` | - | Fehler-Code #1 im Fehlerspreicher (ältester) |
| `error_1_time` | Zeitstempel | Fehlerzeitpunkt #1 im Fehlerspeicher (ältester) |
| `error_2_code` | - | Fehler-Code #2 im Fehlerspreicher |
| `error_2_time` | Zeitstempel | Fehlerzeitpunkt #2 im Fehlerspeicher |
| `error_3_code` | - | Fehler-Code #3 im Fehlerspreicher |
| `error_3_time` | Zeitstempel | Fehlerzeitpunkt #3 im Fehlerspeicher |
| `error_4_code` | - | Fehler-Code #4 im Fehlerspreicher |
| `error_4_time` | Zeitstempel | Fehlerzeitpunkt #4 im Fehlerspeicher |
| `error_5_code` | - | Fehler-Code #5 im Fehlerspreicher (neuester) |
| `error_5_time` | Zeitstempel | Fehlerzeitpunkt #5 im Fehlerspeicher (neuester) |
| `deactivation_1_code` | - | Abschalt-Code #1 im Abschaltungsspeicher (ältester) |
| `deactivation_1_time` | Zeitstempel | Abschaltzeitpunkt #1 im Abschaltungsspeicher (ältester) |
| `deactivation_2_code` | - | Abschalt-Code #2 im Abschaltungsspeicher |
| `deactivation_2_time` | Zeitstempel | Abschaltzeitpunkt #2 im Abschaltungsspeicher |
| `deactivation_3_code` | - | Abschalt-Code #3 im Abschaltungsspeicher |
| `deactivation_3_time` | Zeitstempel | Abschaltzeitpunkt #3 im Abschaltungsspeicher |
| `deactivation_4_code` | - | Abschalt-Code #4 im Abschaltungsspeicher |
| `deactivation_4_time` | Zeitstempel | Abschaltzeitpunkt #4 im Abschaltungsspeicher |
| `deactivation_5_code` | - | Abschalt-Code #5 im Abschaltungsspeicher (neuester) |
| `deactivation_5_time` | Zeitstempel | Abschaltzeitpunkt #5 im Abschaltungsspeicher (neuester) |

Detaillierte Informationen zu den Konfigurationsmöglichkeiten der einzelnen Elemente findest du in der Dokumentation der [ESPHome Textsensorkomponenten](https://www.esphome.io/components/text_sensor).

Einige der Textsensoren liefern einen festen Satz an vordefinierten Werten. Diese Werte können mit einem Lookup-Filter in übersetzten Text umgewandelt werden. Die möglichen Werte werden im Folgenden beschrieben.

Der Sensor `device_type` bietet die folgenden Werte:

| Wert | Beschreibung |
| ---- | ------------ |
| `ERC` | Nicht belegt |
| `SW1` | Sole/Wasser 1 Verdichter |
| `SW2` | Sole/Wasser 2 Verdichter |
| `WW1` | Wasser/Wasser 1 Verdichter |
| `WW2` | Wasser/Wasser 2 Verdichter |
| `L1I` | Luft/Wasser 1 Verdichter Innenaufstellung |
| `L2I` | Luft/Wasser 2 Verdichter Innenaufstellung |
| `L1A` | Luft/Wasser 1 Verdichter Außenaufstellung |
| `L2A` | Luft/Wasser 2 Verdichter Außenaufstellung |
| `KSW` | Kompaktheizzentrale Sole/Wasser |
| `KLW` | Kompaktheizzentrale Luft/Wasser |
| `SWC` | Sole/Wasser Compact |
| `LWC` | Luft/Wasser Compact |
| `L2G` | Luft/Wasser Großgerät 2 Verdichter |
| `WZS` | Wärmezentrale Sole/Wasser |

Der Sensor `bivalence_level` bietet die folgenden Werte:

| Wert | Beschreibung |
| ---- | ------------ |
| `single_compressor` | Ein Verdichter darf laufen |
| `dual_compressor` | Zwei Verdichter dürfen laufen |
| `additional_heater` | Zusätzlicher Wärmeerzeuger darf mitlaufen |

Der Sensor `operational_state` bietet die folgenden Werte:

| Wert | Beschreibung |
| ---- | ------------ |
| `standby` | Bereitschaft |
| `heat` | Heizen |
| `hot_water` | Warmwasserzubereitung |
| `defrost` | Abtauen |
| `swimming_pool` | Schwimmbad |
| `provider_lock` | EVU-Sperre |

Die Sensoren `heating_mode` und `hot_water_mode` bieten die folgenden Werte:

| Wert | Beschreibung |
| ---- | ------------ |
| `auto` | Automatik |
| `second_heater` | Zweiter Wärmeerzeuger |
| `party` | Party |
| `vacation` | Ferien |
| `off` | Aus |

##### Beispiel
```yaml
text_sensor:
  - platform: luxtronik_v1
    device_type:
      name: Anlagentyp
      filters:
        map:
          - LWC -> Luft/Wasser Compact
    firmware_version:
      name: Firmware-Version
    bivalence_level:
      name: Bivalenzstufe
      filters:
        map:
          - single_compressor -> Ein Verdichter darf laufen
          - dual_compressor -> Zwei Verdichter dürfen laufen
          - additional_heater -> Zusätzlicher Wärmeerzeuger darf mitlaufen
    operational_state:
      name: Betriebszustand
      filters:
        map:
          - standby -> Bereitschaft
          - heat -> Heizen
          - hot_water -> Warmwasserzubereitung
          - defrost -> Abtauen
          - swimming_pool -> Schwimmbad
          - provider_lock -> EVU-Sperre
```

## Luxtronik-Konfiguration
Um die serielle Schnittstelle des Luxtronik V1 Heizungssteuergeräts nutzen zu können, muss diese zunächst freigeschaltet werden. Dazu musst du im Menü der Luxtronik Benutzerschnittstelle zu _Service_ -> _Einstellungen_ -> _Datenzugang_ navigieren und die PIN `9445` eingeben. Daraufhin navigiere zu _Service_ -> _Diagnoseprogramme_ und aktiviere die Option "Standard".

-----

# ESPHome Luxtronik V1 (English)
Luxtronik V1 is an ESPHome component to build an ESP firmware for integrating a Luxtronik V1 heating control unit into your smart home. It is primarily intended for integration into Home Assistant, but integration with an alternative home automation software can also be realized via MQTT. The Luxtronik heating control unit in version 1 (V1) does not have a network interface, but only provides an RS-232 interface. A microcontroller is therefore required as a gateway between the heating control unit and the network.

This project was heavily inspired by the [Luxtronik V1 ESPHome component](https://github.com/CBrosius/luxtronik_v1) from [CBrosius](https://github.com/CBrosius). Many thanks to CBrosius for the great work that helped me a lot to understand the Luxtronik interface as well as how ESPHome components work.

- [Disclaimer](#disclaimer)
- [License](LICENSE)
- [Hardware Setup](#hardware-setup)
- [Software Setup](#software-setup)
  - [Luxtronik Component](#luxtronik-component)
  - [UART Component](#uart-component)
  - [Time Component](#time-component)
  - [API/MQTT Component](#apimqtt-component)
  - [WiFi Component](#wifi-component)
  - [Sensors](#sensors)
    - [Numeric Sensors](#numeric-sensors)
    - [Binary Sensors](#binary-sensors)
    - [Text Sensors](#text-sensors)
- [Luxtronik Configuration](#luxtronik-configuration)
- [Help/Support](SUPPORT.md#-getting-support-for-esphome-luxtronik-v1)
- [Contributing](CONTRIBUTING.md#contributing-to-esphome-luxtronik-v1)
- [Known Issues](https://github.com/jensrossbach/esphome-luxtronik-v1/issues?q=is%3Aissue+is%3Aopen+label%3A%22known+issue%22)

## Disclaimer
**THIS SOFTWARE AND ITS AUTHOR ARE NOT AFFILIATED WITH AIT / ALPHA INNOTEC.**

**THE SOFTWARE (INCLUDING THE DOCUMENTATION WITH THE EXAMPLE HARDWARE SETUP) IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.**

## Hardware Setup
Although this project mainly focusses on the software implementation, an example hardware setup is also documented here. There are different possibilities for the setup in regards to the used microcontroller (e.g., ESP8266 or ESP32) and the way how the microcontroller is connected to the Luxtronik heating control unit.

> [!TIP]
> In principle, both an ESP8266 and an ESP32 can be used. The latter is more suitable as it has several hardware UARTs. The ESP8266, on the other hand, only has a single bi-directional hardware UART, which is used by default for logging via USB. When using an ESP8266, you must therefore either do without USB logging or use software UART (which is less reliable).

The ESP microcontroller cannot be directly connected to the RS-232 interface of the Luxtronik heating control unit due to different voltage levels. Instead, a MAX3232 RS-232 line driver/receiver is needed in order to convert between the different voltage levels on Luxtronik and microcontroller side. There are ready-to-use serial to TTL converter modules equipped with a MAX3232 IC and a DE-9 connector available. On the RS-232 side, these modules have a (usually female) DE-9 connector (having only GND, RX and TX pins connected) and on the TTL side, they have 4 pins - VCC and GND for powering the chip as well as RX and TX for the data transfer. Those pins must be connected to appropriate pins of the microcontroller. The following breadboard schematic shows an example test setup using an ESP32 NodeMCU development board as microcontroller.

> [!IMPORTANT]
> Connect the VCC pin to the **3V3 pin** of the ESP microcontroller and **not** to the 5V pin. This ensures that the RX and TX pins also have a voltage level of 3.3V and the GPIO pins of the microcontroller don't get damaged.

![Breadboard Schematic](img/breadboard_schematic.png)

Usually you want to place the microcontroller outside of the heat pump housing for better Wi-Fi reception and easier maintainability. There are two possibilities where to install the "long" wiring between the microcontroller part and the Luxtronik:

1. Serial cable (pre-built or self-built) connected between the MAX3232 module and the RS-232 interface of the Luxtronik
2. 4-wire cable between the microcontroller and the MAX3232 module (and connect the latter directly to the RS-232 interface of the Luxtronik)

Most setups described on the Internet use option 1 but I decided in my case for option 2. As cable, I used a standard German 4-wire telephone installation cable. These cables have two twisted wire pairs (red + black and white + yellow). The red and black wires are predestinated to be used for VCC and GND respectively. Finally, I chose yellow to use for RX and white to use for TX (the same coloring as used in the schematic). The wires have a diameter of 0.6 mm so that the voltage drop at 3.3V and the very low current is negligible for a cable length of 2-4 meters.

Why did I chose option 2? The reason was that ready-to-use serial cables are hard to get and expensive nowadays and - the more significant reason - the big connector ends do not fit through the small outlets in the heat pump housing. I also didn't want to build my own cable as also DE-9 connectors are hard to get and expensive. Using option 2, I soldered the wires directly to the TTL pins of the MAX3232 board on one side and connected the board directly to the Luxtronik RS-232 interface (having a small null modem adapter between, see tip below). On the microcontroller side, I soldered pin headers as well as PCB mount screw terminal blocks on a stripboard and connected the appropriate pins. I plugged an ESP32 NodeMCU development board into the pin headers and was able to easily connect the wires of the telephone cable to the screw terminal blocks after I fed the cable through the outlets of the heat pump housing.

> [!TIP]
> Most resources on the Internet state that the serial cable to connect the receiver with the Luxtronik heating control unit has to be a cable with a one-to-one assignment of RX and TX pins. However, this seems to depend on the serial interface that is used on receiver side. In my case, I had to use a null modem adapter, which flips RX and TX pins, in order to get it working.

There are similar projects for reading out the serial interface of the Luxtronik V1 heating control unit existing. These projects have good documentation of the required hardware setup, so please also have a look to these helpful resources on the Internet:

- https://github.com/CBrosius/luxtronik_v1
- https://wiki.fhem.de/wiki/Luxtronik_1_in_FHEM (not ESPHome based)

## Software Setup
To build an ESPHome firmware, you have to create a YAML based configuration file. You can use the [example configuration file](example_config/luxtronik_lwc.yaml) provided in this repository as a starting point and adapt it to your needs. For more information about writing ESPHome firmware configuration files, please refer to the [ESPHome documentation](https://www.esphome.io).

The following sections describe the most notable components contained in the firmware configuration file.

### Luxtronik Component
The Luxtronik component is essential and must be added in order to use its sensors.

As this is a custom component which is not part of ESPHome, it must be imported explicitly. The easiest way is to load the component directly from this repository.

##### Example

```yaml
external_components:
  - source: github://jensrossbach/esphome-luxtronik-v1
    components: [luxtronik_v1]
```

The following generic configuration items can be configured:

| Option | Mandatory | Default Value | Description |
| ------ | --------- | ------------- |------------ |
| `uart_id` | yes | n/a | ID of the configured UART component (see below) |
| `update_interval` | no | 60s | The interval how often the component fetches data from the heating control unit |

##### Example
```yaml
luxtronik_v1:
  uart_id: uart_bus
  update_interval: 300s  # update every 5 minutes
```

### UART Component
A [UART component](https://www.esphome.io/components/uart.html) is mandatory for the Luxtronik V1 component to function correctly and must therefore always be added. The serial communication parameters (baud rate, data bits, stop bits and parity) are inherently defined by the Luxtronik V1 heating control unit and must not be changed therefore. Only the RX and TX pins can be adapted depending on the used hardware.

| Parameter | Value       |
| --------- | ----------- |
| Baud rate | 57600 bit/s |
| Data bits | 8           |
| Stop bits | 1           |
| Parity    | None        |

##### Example
See below the example for an ESP32 NodeMCU using the UART2:

```yaml
uart:
  id: uart_bus
  rx_pin: GPIO16    # UART2 RX
  tx_pin: GPIO17    # UART2 TX
  baud_rate: 57600
  data_bits: 8
  stop_bits: 1
  parity: NONE
```

### Time Component
A [time component](https://www.esphome.io/components/time) is not strictly required but it is recommended in case you want to use the error and deactivation sensors. These sensors provide timestamps and to have the correct time zone applied, a time component has to be added.

##### Example
See below example which uses Home Assistant as time source:

```yaml
time:
  - platform: homeassistant
    id: ha_time
```

### API/MQTT Component
An [API component](https://www.esphome.io/components/api.html) is required if the ESP shall be integrated into Home Assistant. For the case that an alternative home automation software shall be used, a [MQTT component](https://www.esphome.io/components/mqtt.html) has to be added instead.

##### Example
See below example for the integration into Home Assistant (with encrypted API):

```yaml
api:
  encryption:
    key: !secret ha_api_key
```

And below an example for usage with an alternative home automation software via MQTT:

```yaml
mqtt:
  broker: 10.0.0.2
  username: !secret mqtt_user
  password: !secret mqtt_password
```

### WiFi Component
A [WiFi component](https://www.esphome.io/components/wifi.html) should be present, as otherwise the sensor values cannot be easily transmitted to another computer.

##### Example

```yaml
wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
```

### Sensors
The Luxtronik V1 component provides various sensors which expose data from the heating control unit. All sensors are optional and can be omitted if not needed. Depending on the type of data provided by the Luxtronik heating control unit, either numeric, binary or text sensors are used to expose the values.

#### Numeric Sensors
The following numeric sensors can be configured:

| Sensor | Device Class | Description |
| ------ | ------------ | ----------- |
| `flow_temperature` | Temperature | Flow temperature of heating circuit |
| `return_temperature` | Temperature | Return temperature of heating circuit |
| `return_set_temperature` | Temperature | Return set-temperature of heating circuit |
| `hot_gas_temperature` | Temperature | Hot gas temperature |
| `outside_temperature` | Temperature | Outside temperature |
| `hot_water_temperature` | Temperature | Temperature of hot water |
| `hot_water_set_temperature` | Temperature | Set-temperature of hot water |
| `heat_source_input_temperature` | Temperature | Temperature of heat source input |
| `heat_source_output_temperature` | Temperature | Temperature of heat source output |
| `mixed_circuit_1_temperature` | Temperature | Temperature of mixed circuit 1 |
| `mixed_circuit_1_set_temperature` | Temperature | Set-emperature of mixed circuit 1 |
| `remote_adjuster_temperature` | Temperature | Temperature of the remote adjuster |

For detailed configuration options of each item, please refer to ESPHome [sensor component configuration](https://www.esphome.io/components/sensor).

##### Example
```yaml
sensor:
  - platform: luxtronik_v1
    flow_temperature:
      name: Ist-Temperatur Vorlauf Heizkreis
    return_temperature:
      name: Ist-Temperatur Rücklauf Heizkreis
    return_set_temperature:
      name: Soll-Temperatur Rücklauf Heizkreis
```

#### Binary Sensors
The following binary sensors can be configured:

| Sensor | Device Class | Description |
| ------ | ------------ | ----------- |
| `defrost_brine_flow` | - | Defrost / brine pressure / flow rate (depending on type of device) |
| `power_provider_lock_period` | Lock | Power provider lock period |
| `low_pressure_state` | Problem | Low pressure state |
| `high_pressure_state` | Problem | High pressure state |
| `engine_protection` | Problem | Engine protection |
| `external_power` | - | External power anode |
| `defrost_valve` | - | Defrost valve |
| `hot_water_pump` | Running | Hot water circulation pump |
| `heating_pump` | Running | Heating circulation pump |
| `floor_heating_pump` | Running | Floor heating circulation pump |
| `housing_ventilation` | Running | Ventilation for housing |
| `ventilation_pump` | Running | Ventilation / well or brine circulation pump (depending on type of device) |
| `compressor_1` | Running | Compressor 1 in heat pump |
| `compressor_2` | Running | Compressor 2 in heat pump |
| `extra_pump` | Running | Additional circulation pump |
| `secondary_heater_1` | Running | Secondary heater |
| `secondary_heater_2_failure` | Problem | Secondary heater error collector |

Additionally, the following diagnostic binary sensor can be configured:

| Sensor | Device Class | Description |
| ------ | ------------ | ----------- |
| `device_communication` | Problem | Indicates if the communication with the heating control unit operates properly |

For detailed configuration options of each item, please refer to ESPHome [binary sensor component configuration](https://www.esphome.io/components/binary_sensor).

##### Example
```yaml
binary_sensor:
  - platform: luxtronik_v1
    hot_water_pump:
      name: Brauchwarmwasserumwälzpumpe
    floor_heating_pump:
      name: Fußbodenheizungsumwälzpumpe
    ventilation_pump:
      name: Ventilator
    compressor_1:
      name: Verdichter
    power_provider_lock_period:
      name: Sperrzeit EVU
    device_communication:
      name: Gerätekommunikation
```

#### Text Sensors
The following text sensors can be configured:

| Sensor | Device Class | Description |
| ------ | ------------ | ----------- |
| `device_type` | - | Type of heat pump device |
| `firmware_version` | - | Version of the Luxtronik V1 firmware |
| `bivalence_level` | - | Bivalence level |
| `operational_state` | - | State of operation |
| `heating_mode` | - | Heating mode |
| `hot_water_mode` | - | Hot water mode |
| `mixer_1_state` | - | State of mixer 1 |
| `error_1_code` | - | Error code #1 in error memory (oldest) |
| `error_1_time` | Timestamp | Error timestamp #1 in error memory (oldest) |
| `error_2_code` | - | Error code #2 in error memory |
| `error_2_time` | Timestamp | Error timestamp #2 in error memory |
| `error_3_code` | - | Error code #3 in error memory |
| `error_3_time` | Timestamp | Error timestamp #3 in error memory |
| `error_4_code` | - | Error code #4 in error memory |
| `error_4_time` | Timestamp | Error timestamp #4 in error memory |
| `error_5_code` | - | Error code #5 in error memory (newest) |
| `error_5_time` | Timestamp | Error timestamp #5 in error memory (newest) |
| `deactivation_1_code` | - | Deactivation code #1 in deaktivation memory (oldest) |
| `deactivation_1_time` | Timestamp | Deactivation timestamp #1 in deaktivation memory (oldest) |
| `deactivation_2_code` | - | Deactivation code #2 in deaktivation memory |
| `deactivation_2_time` | Timestamp | Deactivation timestamp #2 in deaktivation memory |
| `deactivation_3_code` | - | Deactivation code #3 in deaktivation memory |
| `deactivation_3_time` | Timestamp | Deactivation timestamp #3 in deaktivation memory |
| `deactivation_4_code` | - | Deactivation code #4 in deaktivation memory |
| `deactivation_4_time` | Timestamp | Deactivation timestamp #4 in deaktivation memory |
| `deactivation_5_code` | - | Deactivation code #5 in deaktivation memory (newest) |
| `deactivation_5_time` | Timestamp | Deactivation timestamp #5 in deaktivation memory (newest) |

For detailed configuration options of each item, please refer to ESPHome [text sensor component configuration](https://www.esphome.io/components/text_sensor).

Some of the text sensors provide a fixed set of predefined values. These values can be mapped to translated text using a [lookup filter](https://www.esphome.io/components/text_sensor/#map). The possible values are described below.

The `device_type` sensor provides the following values:

| Value | Description |
| ----- | ----------- |
| `ERC` | Not used |
| `SW1` | Sole/Wasser 1 Verdichter |
| `SW2` | Sole/Wasser 2 Verdichter |
| `WW1` | Wasser/Wasser 1 Verdichter |
| `WW2` | Wasser/Wasser 2 Verdichter |
| `L1I` | Luft/Wasser 1 Verdichter Innenaufstellung |
| `L2I` | Luft/Wasser 2 Verdichter Innenaufstellung |
| `L1A` | Luft/Wasser 1 Verdichter Außenaufstellung |
| `L2A` | Luft/Wasser 2 Verdichter Außenaufstellung |
| `KSW` | Kompaktheizzentrale Sole/Wasser |
| `KLW` | Kompaktheizzentrale Luft/Wasser |
| `SWC` | Sole/Wasser Compact |
| `LWC` | Luft/Wasser Compact |
| `L2G` | Luft/Wasser Großgerät 2 Verdichter |
| `WZS` | Wärmezentrale Sole/Wasser |

The `bivalence_level` sensor provides the following values:

| Value | Description |
| ----- | ----------- |
| `single_compressor` | One compressor may run |
| `dual_compressor` | Two compressors may run |
| `additional_heater` | Additional heater may run |

The `operational_state` sensor provides the following values:

| Value | Description |
| ----- | ----------- |
| `standby` | Stand-by |
| `heat` | Heating |
| `hot_water` | Hot water preparation |
| `defrost` | Defrosting |
| `swimming_pool` | Swimming pool |
| `provider_lock` | Energy provider lock period |

The sensors `heating_mode` and `hot_water_mode` provide the following values:

| Value | Description |
| ---- | ------------ |
| `auto` | Automatic mode |
| `second_heater` | Second heater mode |
| `party` | Party mode |
| `vacation` | Vacation mode |
| `off` | Off |

##### Example
```yaml
text_sensor:
  - platform: luxtronik_v1
    device_type:
      name: Anlagentyp
      filters:
        map:
          - LWC -> Luft/Wasser Compact
    firmware_version:
      name: Firmware-Version
    bivalence_level:
      name: Bivalenzstufe
      filters:
        map:
          - single_compressor -> Ein Verdichter darf laufen
          - dual_compressor -> Zwei Verdichter dürfen laufen
          - additional_heater -> Zusätzlicher Wärmeerzeuger darf mitlaufen
    operational_state:
      name: Betriebszustand
      filters:
        map:
          - standby -> Bereitschaft
          - heat -> Heizen
          - hot_water -> Warmwasserzubereitung
          - defrost -> Abtauen
          - swimming_pool -> Schwimmbad
          - provider_lock -> EVU-Sperre
```

## Luxtronik Configuration
In order to use the serial interface of the Luxtronik V1 heating control unit, it needs to be unlocked first. To do this, navigate to _Service_ -> _Einstellungen_ -> _Datenzugang_ and enter the PIN `9445`. After that, navigate to _Service_ -> _Diagnoseprogramme_ and activate the option "Standard".
