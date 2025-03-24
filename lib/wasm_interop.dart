import 'dart:async';
import 'dart:js' as js;
import 'dart:js_util' as js_util;

/// A class to interact with the RISC-V simulator WebAssembly module
class RiscVSimulator {
  /// The JavaScript RiscVSimulator instance.
  late js.JsObject _simulator;

  // Private constructor
  RiscVSimulator._internal();

  /// Factory method to asynchronously create an instance of [RiscVSimulator].
  static Future<RiscVSimulator> create() async {
    // Wait for the WASM module to be loaded and initialized.
    await _waitForModule();
    
    // Retrieve the RiscVSimulator constructor from the module.
    final simConstructor = js.context['Module']['RiscVSimulator'];
    if (simConstructor == null) {
      throw Exception("RiscVSimulator constructor not found in Module.");
    }
    // Create a new instance.
    var simulator = RiscVSimulator._internal();
    simulator._simulator = js.JsObject(simConstructor, []);
    print('Simulator instance: ${simulator._simulator}');
    print('Has init? ${js_util.hasProperty(simulator._simulator, 'init')}');
    
    print("WebAssembly module and RiscVSimulator are loaded and ready.");
    return simulator;
  }

  /// Helper function that waits until the WASM module is loaded and ready.
  static Future<void> _waitForModule() async {
    // Poll every 100ms until window.Module and its RiscVSimulator are defined.
    while (js.context['Module'] == null ||
           !js.context['Module'].hasProperty('RiscVSimulator')) {
      await Future.delayed(Duration(milliseconds: 100));
    }
  }

  /// Initialize the simulator.
  void init() {
    _simulator.callMethod('init', []);
  }

  /// Clean up resources.
  void cleanup() {
    _simulator.callMethod('cleanup', []);
  }

  /// Load machine code into memory.
  void loadCode(String code) {
    _simulator.callMethod('loadCode', [code]);
  }

  /// Execute a single step.
  bool step() {
    return _simulator.callMethod('step', []);
  }

  /// Run the entire program.
  void run() {
    _simulator.callMethod('run', []);
  }

  /// Reset the simulator.
  void reset() {
    _simulator.callMethod('reset', []);
  }

  /// Get register values as a string.
  /// Returns a semicolon-separated list of register values in format "x0:00000000;x1:00000000;..."
  String showReg() {
    return _simulator.callMethod('showReg', []);
  }

  /// Get memory values as a string.
  /// [segment]: "text", "static", or "dynamic"
  /// [startAddr]: starting address
  /// [count]: number of words to retrieve
  /// Returns a semicolon-separated list of memory values in format "addr:value;addr:value;..."
  String showMem(String segment, int startAddr, int count) {
    return _simulator.callMethod('showMem', [segment, startAddr, count]);
  }

  /// Get current PC value.
  String getPC() {
    return _simulator.callMethod('getPC', []);
  }

  /// Get console output.
  String getConsoleOutput() {
    return _simulator.callMethod('getConsoleOutput', []);
  }

  /// Clear console output.
  void clearConsoleOutput() {
    _simulator.callMethod('clearConsoleOutput', []);
  }

  /// Get cycle count.
  int getCycleCount() {
    return _simulator.callMethod('getCycleCount', []);
  }

  /// Get instruction count.
  int getInstructionCount() {
    return _simulator.callMethod('getInstructionCount', []);
  }
}
