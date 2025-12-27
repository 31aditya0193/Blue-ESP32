//
//  ContentView.swift
//  Blue
//
//  Created by Aditya Mishra on 25/12/25.
//

import SwiftUI
import CoreBluetooth
import Combine

// BLE UUIDs (match ESP32)
let serviceUUID = CBUUID(string: "4fafc201-1fb5-459e-8fcc-c5c9c331914b")
let ledCharUUID = CBUUID(string: "beb5483e-36e1-4688-b7f5-ea07361b26a8")
let textCharUUID = CBUUID(string: "1c95d5e3-d8f7-413a-bf3d-7d3d930a6d48")

struct ContentView: View {
    @StateObject private var bluetoothManager = BluetoothManager()
    @State private var textToSend = ""
    @State private var selectedColor: Color = .black
    
    var body: some View {
        VStack(spacing: 20) {
            Text("ESP32 Controller")
                .font(.headline)
            
            if bluetoothManager.isConnected {
                // Color Picker for LED
                ColorPicker("Select LED Color", selection: $selectedColor)
                    .onChange(of: selectedColor) { oldValue, newValue in
                        bluetoothManager.sendColor(newValue)
                    }
                
                // Text Input for Screen
                TextField("Enter text for screen", text: $textToSend)
                Button("Send Text") {
                    bluetoothManager.sendText(textToSend)
                    textToSend = ""
                }
            } else {
                Text("Scanning for ESP32...")
            }
        }
        .padding()
        .onAppear {
            bluetoothManager.startScanning()
        }
    }
}


#Preview {
    ContentView()
}
