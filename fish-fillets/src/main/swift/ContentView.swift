import SwiftUI

import SwiftUI
import AVFAudio

struct ContentView: UIViewControllerRepresentable {
    func makeUIViewController(context: Context) -> some UIViewController {
        return ContentViewController()
    }
    
    func updateUIViewController(_ uiView: UIViewControllerType, context: Context) {
        
    }
}

class MetalView: UIView {
    override class var layerClass: AnyClass {
        return CAMetalLayer.self;
    }
}

class ContentViewController: UIViewController {
    var ptr : UnsafeMutableRawPointer? = nil
    //var tou : ToneOutputUnit = ToneOutputUnit()
    var audioUnit : AUAudioUnit? = nil
    
    override func loadView() {
        self.view = MetalView()
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        if event?.allTouches?.count == 1 {
            let loc = touches.first!.location(in: self.view)
            touchEvent(ptr, kTouchEventPointerDown, Float(loc.x), Float(loc.y))
        }
    }
    
    override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
        touchEvent(ptr, kTouchEventCancel, 0, 0)
    }
    
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        if event?.allTouches?.count == 1 {
            let loc = touches.first!.location(in: self.view)
            touchEvent(ptr, kTouchEventPointerMove, Float(loc.x), Float(loc.y))
        }
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        if event?.allTouches?.count == 1 {
            let loc = touches.first!.location(in: self.view)
            touchEvent(ptr, kTouchEventPointerUp, Float(loc.x), Float(loc.y))
        }
    }
    
    override func viewDidLoad() {
        NSLog("viewDidLoad")
        NSLog("bundle loc: %@", Bundle.main.preferredLocalizations.first!);
        let nc = NotificationCenter.default
        nc.addObserver(self, selector: #selector(enterForeground), name: UIScene.willEnterForegroundNotification, object: nil)
        nc.addObserver(self, selector: #selector(enterBackground), name: UIScene.didEnterBackgroundNotification, object: nil)
        nc.addObserver(self, selector: #selector(enterActive), name: UIScene.didActivateNotification, object: nil)
        nc.addObserver(self, selector: #selector(enterInactive), name: UIScene.willDeactivateNotification, object: nil)
        let layer = self.view.layer as! CAMetalLayer;
        ptr = start(Unmanaged.passUnretained(layer).toOpaque())
        do {
            let audioSession = AVAudioSession.sharedInstance()
            try audioSession.setCategory(AVAudioSession.Category.playback)
            let sampleRate = 22050.0
            let duration = 256.0 / sampleRate
            try audioSession.setPreferredSampleRate(sampleRate)
            try audioSession.setPreferredIOBufferDuration(duration)
            NotificationCenter.default.addObserver(self, selector: #selector(audioInterrupted), name: AVAudioSession.interruptionNotification, object: audioSession)
            try audioSession.setActive(true)
            // ----
            let audioComponent = AudioComponentDescription(
                componentType: kAudioUnitType_Output,
                componentSubType: kAudioUnitSubType_RemoteIO,
                componentManufacturer: kAudioUnitManufacturer_Apple,
                componentFlags: 0, componentFlagsMask: 0)
            audioUnit = try AUAudioUnit(componentDescription: audioComponent)
            let bus0 = audioUnit!.inputBusses[0]
            let audioFormat = AVAudioFormat(
                commonFormat: AVAudioCommonFormat.pcmFormatFloat32, sampleRate: sampleRate, channels: AVAudioChannelCount(1), interleaved: false)
            try bus0.setFormat(audioFormat!)
            let iptr = ptr;
            audioUnit!.outputProvider = { (actionFlags, timestamp, frameCount, inputBusNumber, inputDataList) -> AUAudioUnitStatus in
                renderAudio(iptr, frameCount, inputDataList[0].mBuffers.mData)
                return 0
            }
            audioUnit!.isOutputEnabled = true // should be default
            try audioUnit!.allocateRenderResources()
            try audioUnit!.startHardware()
        } catch let error {
            NSLog("error %s", error.localizedDescription)
        }
    }
    
    override func viewWillAppear(_ animated: Bool) {
        NSLog("viewWillAppear")
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        NSLog("viewDidDisappear")
    }
    
    @objc func enterForeground() {
        NSLog("foreground")
    }
    
    @objc func enterActive() {
        NSLog("active")
        //activate(ptr)
    }
    
    @objc func enterInactive() {
        NSLog("inactive")
        //inactivate(ptr)
    }
    
    @objc func enterBackground() {
        NSLog("background")
    }
    
    @objc func audioInterrupted() {
        NSLog("audio interrupted")
    }
}
