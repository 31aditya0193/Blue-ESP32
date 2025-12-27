//
//  BluetoothManager.swift
//  Blue
//
//  Created by Aditya Mishra on 25/12/25.
//

import Combine
import CoreBluetooth
import SwiftUI

class BluetoothManager: NSObject, ObservableObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    private var centralManager: CBCentralManager!
    private var peripheral: CBPeripheral?
    
    @Published var isConnected = false
    
    override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    func startScanning() {
        if centralManager.state == .poweredOn {
            centralManager.scanForPeripherals(withServices: [serviceUUID], options: nil)
        }
    }
    
    // CBCentralManagerDelegate
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
        case .unknown: print("State: unknown (initializing)")
        case .resetting: print("State: resetting")
        case .poweredOn: print("State: poweredOn – ready!")
        case .poweredOff: print("State: poweredOff – turn on Bluetooth")
        case .unauthorized: print("State: unauthorized – check privacy settings")
        case .unsupported: print("State: unsupported – old device?")
        @unknown default: print("State: unexpected")
        }
        if central.state == .poweredOn {
            startScanning()
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        if peripheral.name == "ESP32_RGB_Display" {  // Match device name
            self.peripheral = peripheral
            peripheral.delegate = self
            centralManager.stopScan()
            centralManager.connect(peripheral, options: nil)
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        isConnected = true
        peripheral.discoverServices([serviceUUID])
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        isConnected = false
        startScanning()
    }
    
    // CBPeripheralDelegate
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        guard let services = peripheral.services else { return }
        for service in services {
            peripheral.discoverCharacteristics([ledCharUUID, textCharUUID], for: service)
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        // Characteristics are now ready for writing
    }
    
    // Send Functions
    func sendColor(_ color: Color) {
        guard let peripheral = peripheral,
              let ledChar = findCharacteristic(ledCharUUID, in: peripheral) else { return }
        
        var red: CGFloat = 0, green: CGFloat = 0, blue: CGFloat = 0
        UIColor(color).getRed(&red, green: &green, blue: &blue, alpha: nil)
        let data = Data([UInt8(red * 255), UInt8(green * 255), UInt8(blue * 255)])
        peripheral.writeValue(data, for: ledChar, type: .withResponse)
    }
    
    func sendText(_ text: String) {
        guard let peripheral = peripheral,
              let textChar = findCharacteristic(textCharUUID, in: peripheral) else { return }
        
        let data = Data(text.utf8)
        peripheral.writeValue(data, for: textChar, type: .withResponse)
    }
    
    private func findCharacteristic(_ uuid: CBUUID, in peripheral: CBPeripheral) -> CBCharacteristic? {
        guard let service = peripheral.services?.first(where: { $0.uuid == serviceUUID }),
              let char = service.characteristics?.first(where: { $0.uuid == uuid }) else { return nil }
        return char
    }
}
