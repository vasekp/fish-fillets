import SwiftUI

import SwiftUI
import AVFAudio

struct ContentView: UIViewControllerRepresentable {
    func makeUIViewController(context: Context) -> some UIViewController {
        return ContentViewController()
    }
    
    func updateUIViewController(_ uiView: UIViewControllerType, context: Context) { }
}

class MetalView: UIView {
    override class var layerClass: AnyClass {
        return CAMetalLayer.self;
    }
}

class ContentViewController: UIViewController {
    var ptr : UnsafeMutableRawPointer? = nil
    var audioUnit : AUAudioUnit? = nil
    var scaleFactor : CGFloat = 1.0
    
    override func loadView() {
        self.view = MetalView()
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        let fullCount = event?.allTouches?.count
        if fullCount == 1 {
            let loc = touches.first!.location(in: self.view)
            touchEvent(ptr, kTouchEventPointerDown, Float(loc.x * scaleFactor), Float(loc.y * scaleFactor))
        } else if fullCount == 2 {
            touchEvent(ptr, kTouchEventTwoTap, 0, 0)
        }
    }
    
    override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
        touchEvent(ptr, kTouchEventCancel, 0, 0)
    }
    
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        if event?.allTouches?.count == 1 {
            let loc = touches.first!.location(in: self.view)
            touchEvent(ptr, kTouchEventPointerMove, Float(loc.x * scaleFactor), Float(loc.y * scaleFactor))
        }
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        if event?.allTouches?.count == 1 {
            let loc = touches.first!.location(in: self.view)
            touchEvent(ptr, kTouchEventPointerUp, Float(loc.x * scaleFactor), Float(loc.y * scaleFactor))
        }
    }
    
    override func viewDidLoad() {
        let nc = NotificationCenter.default
        nc.addObserver(self, selector: #selector(enterForeground), name: UIScene.willEnterForegroundNotification, object: nil)
        nc.addObserver(self, selector: #selector(enterBackground), name: UIScene.didEnterBackgroundNotification, object: nil)
        scaleFactor = UIScreen.main.nativeScale
        self.view.contentScaleFactor = scaleFactor
        self.view.isMultipleTouchEnabled = true
        let layer = self.view.layer as! CAMetalLayer;
        ptr = startApp(Unmanaged.passUnretained(layer).toOpaque())
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
            let iptr = ptr
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
        NSLog("viewWillAppear %f %f", self.view.frame.size.width, self.view.frame.size.height);
        resizeWin(ptr, UInt32(self.view.frame.size.width * scaleFactor), UInt32(self.view.frame.size.height * scaleFactor))
    }
    
    @objc func enterForeground() {
        NSLog("foreground")
        setFocus(ptr, 1)
    }
    
    @objc func enterBackground() {
        NSLog("background")
        setFocus(ptr, 0)
    }
    
    @objc func audioInterrupted() {
        NSLog("audio interrupted")
    }
}
