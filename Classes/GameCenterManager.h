//
//  GameCenterManager.h
//  puzzle
//
//  Created by FengZi on 14-2-13.
//
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

@protocol GameCenterManagerDelegate <NSObject>
@optional
- (void) processGameCenterAuth : (NSString*)error;
@end

@interface GameCenterManager :  NSObject <GKLeaderboardViewControllerDelegate, GKAchievementViewControllerDelegate, GKMatchmakerViewControllerDelegate, GKMatchDelegate>{
    BOOL userAuthenticated;
}

@property (nonatomic, assign) id <GameCenterManagerDelegate> delegate;
@property (assign, readonly) BOOL gameCenterAvailable;

+ (GameCenterManager *)getInstance;
- (void) authenticateLocalUser;

- (void) showLeaderboard;
- (void)leaderboardViewControllerDidFinish:(GKLeaderboardViewController *)viewController;
- (void) reportScore: (int64_t) score forCategory: (NSString*) category;
+ (void)goToAppStoreEvaluate;

@end