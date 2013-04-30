//
//  AwfulTabBarController.m
//  Awful
//
//  Copyright 2012 Awful Contributors. CC BY-NC-SA 3.0 US http://github.com/AwfulDevs/Awful
//

#import "AwfulTabBarController.h"
#import "AwfulTabBar.h"

@interface AwfulTabBarController () <AwfulTabBarDelegate, UINavigationControllerDelegate>

@property (weak, nonatomic) AwfulTabBar *tabBar;

@end


@implementation AwfulTabBarController

- (void)setViewControllers:(NSArray *)viewControllers
{
    if (_viewControllers == viewControllers) return;
    _viewControllers = [viewControllers copy];
    for (UINavigationController *nav in _viewControllers) {
        if (![nav isKindOfClass:[UINavigationController class]]) return;
        nav.delegate = self;
    }
    self.tabBar.items = [self.viewControllers valueForKey:@"tabBarItem"];
    self.selectedViewController = self.viewControllers[0];
}

- (void)setSelectedViewController:(UIViewController *)selectedViewController
{
    if (_selectedViewController == selectedViewController) return;
    if (_selectedViewController) {
        [self replaceViewController:_selectedViewController
                 withViewController:selectedViewController];
    } else {
        [self addViewController:selectedViewController];
    }
    _selectedViewController = selectedViewController;
    self.tabBar.selectedItem = selectedViewController.tabBarItem;
}

- (void)addViewController:(UIViewController *)coming
{
    [self addChildViewController:coming];
    coming.view.frame = [self frameForContainedViewController:coming];
    [self.view insertSubview:coming.view belowSubview:self.tabBar];
    [coming didMoveToParentViewController:self];
}

- (void)replaceViewController:(UIViewController *)going
           withViewController:(UIViewController *)coming
{
    [going willMoveToParentViewController:nil];
    [self addChildViewController:coming];
    coming.view.frame = [self frameForContainedViewController:coming];
    [self.view insertSubview:coming.view belowSubview:going.view];
    [going.view removeFromSuperview];
    [going removeFromParentViewController];
    [coming didMoveToParentViewController:self];
}

- (CGRect)frameForContainedViewController:(UIViewController *)viewController
{
    CGRect frame = self.view.bounds;
    if ([viewController isKindOfClass:[UINavigationController class]]) {
        UINavigationController *nav = (id)viewController;
        if (!nav.topViewController.hidesBottomBarWhenPushed) {
            frame.size.height -= CGRectGetHeight(self.tabBar.bounds);
        }
    } else if (!self.tabBar.hidden) {
        frame.size.height -= CGRectGetHeight(self.tabBar.bounds);
    }
    return frame;
}

#pragma mark - UIViewController

- (void)loadView
{
    self.view = [[UIView alloc] initWithFrame:[UIScreen mainScreen].applicationFrame];
    self.view.clipsToBounds = YES;
    CGRect tabBarFrame;
    CGRectDivide(self.view.bounds, &tabBarFrame, &(CGRect){}, 40, CGRectMaxYEdge);
    AwfulTabBar *tabBar = [[AwfulTabBar alloc] initWithFrame:tabBarFrame];
    tabBar.items = [self.viewControllers valueForKey:@"tabBarItem"];
    tabBar.selectedItem = self.selectedViewController.tabBarItem;
    tabBar.delegate = self;
    tabBar.autoresizingMask = (UIViewAutoresizingFlexibleWidth |
                               UIViewAutoresizingFlexibleTopMargin);
    [self.view addSubview:tabBar];
    self.tabBar = tabBar;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
    for (UIViewController *viewController in self.viewControllers) {
        if (![viewController shouldAutorotateToInterfaceOrientation:toInterfaceOrientation]) {
            return NO;
        }
    }
    return YES;
}

- (UIView *)rotatingHeaderView
{
    return self.selectedViewController.rotatingHeaderView;
}

- (UIView *)rotatingFooterView
{
    return self.tabBar.hidden ? self.selectedViewController.rotatingFooterView : self.tabBar;
}

#pragma mark - AwfulTabBarDelegate

- (void)tabBar:(AwfulTabBar *)tabBar didSelectItem:(UITabBarItem *)item
{
    UIViewController *selected = self.viewControllers[[tabBar.items indexOfObject:item]];
    if (![self.delegate tabBarController:self shouldSelectViewController:selected]) {
        self.tabBar.selectedItem = self.selectedViewController.tabBarItem;
        return;
    }
    if ([selected isEqual:self.selectedViewController]) {
        if ([selected isKindOfClass:[UINavigationController class]]) {
            UINavigationController *nav = (id)selected;
            if ([nav.viewControllers count] > 1) {
                [(UINavigationController *)selected popToRootViewControllerAnimated:YES];
            } else if ([nav.topViewController.view isKindOfClass:[UIScrollView class]]) {
                UIScrollView *scrollView = (id)nav.topViewController.view;
                [scrollView setContentOffset:CGPointMake(0, -scrollView.contentInset.top)
                                    animated:YES];
            }
        }
    } else {
        self.selectedViewController = selected;
    }
}

#pragma mark - UINavigationControllerDelegate

- (void)navigationController:(UINavigationController *)navigationController
      willShowViewController:(UIViewController *)viewController
                    animated:(BOOL)animated
{
    if (viewController.hidesBottomBarWhenPushed && !self.tabBar.hidden) {
        self.selectedViewController.view.frame = (CGRect){ .size = self.view.bounds.size };
        CGRect tabBarFrame = CGRectOffset(self.tabBar.frame, -self.tabBar.bounds.size.width, 0);
        if (animated) {
            [UIView animateWithDuration:0.34
                                  delay:0
                                options:(UIViewAnimationOptionLayoutSubviews |
                                         UIViewAnimationOptionCurveEaseIn)
                             animations:^
             {
                 self.tabBar.frame = tabBarFrame;
             } completion:^(BOOL finished) {
                 self.tabBar.hidden = YES;
             }];
        } else {
            self.tabBar.frame = tabBarFrame;
            self.tabBar.hidden = YES;
        }
    } else if (!viewController.hidesBottomBarWhenPushed && self.tabBar.hidden) {
        self.tabBar.hidden = NO;
        CGRect tabBarFrame, containedFrame;
        CGRectDivide(self.view.bounds, &tabBarFrame, &containedFrame,
                     self.tabBar.bounds.size.height, CGRectMaxYEdge);
        if (animated) {
            [UIView animateWithDuration:0.25
                                  delay:0
                                options:(UIViewAnimationOptionLayoutSubviews |
                                         UIViewAnimationOptionCurveEaseIn)
                             animations:^{
                                 self.tabBar.frame = tabBarFrame;
                             } completion:^(BOOL finished) {
                                 [UIView animateWithDuration:0.1
                                                       delay:0
                                                     options:(UIViewAnimationOptionLayoutSubviews |
                                                              UIViewAnimationOptionCurveEaseIn)
                                                  animations:^
                                 {
                                     self.selectedViewController.view.frame = containedFrame;
                                 } completion:nil];
                             }];
        } else {
            self.tabBar.frame = tabBarFrame;
            self.selectedViewController.view.frame = containedFrame;
        }
    }
}

@end


@implementation UIViewController (AwfulTabBarController)

- (AwfulTabBarController *)awfulTabBarController
{
    UIViewController *vc = self;
    while (vc && ![vc isKindOfClass:[AwfulTabBarController class]]) {
        vc = vc.parentViewController;
    }
    return (AwfulTabBarController *)vc;
}

@end
